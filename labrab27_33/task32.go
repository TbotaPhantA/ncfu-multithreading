package main

import (
	"fmt"
	"io"
	"net/http"
	"sync"
)

// Simple in-memory cache (made thread-safe with a mutex)
var cacheResp map[string][]byte = make(map[string][]byte)
var cacheMutex sync.Mutex

func fetchData(w http.ResponseWriter, path string, isRequestCompleted chan bool) {
	target := "https://google.com/"

	client := &http.Client{}
	req, err := http.NewRequest(http.MethodGet, target+path, nil)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	resp, err := client.Do(req)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadGateway)
		return
	}
	defer resp.Body.Close()

	body, err := io.ReadAll(resp.Body)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	// Cache the response in a thread-safe manner
	cacheMutex.Lock()
	cacheResp[path] = body
	cacheMutex.Unlock()

	// Write response to client
	w.WriteHeader(resp.StatusCode)
	w.Write(body)
	fmt.Println("Fetched from Google:", path)
	isRequestCompleted <- true
}

func handleIncomingRequest(w http.ResponseWriter, r *http.Request) {
	// Check for cached response in a thread-safe manner
	cacheMutex.Lock()
	path := r.URL.String()
	data, ok := cacheResp[path]
	cacheMutex.Unlock()

	if ok {
		w.WriteHeader(http.StatusOK)
		w.Write(data)
		fmt.Println("Serving from cache:", path)
		return
	}

	// Fetch from Google in a separate goroutine
	isRequestCompleted := make(chan bool)
	go fetchData(w, path, isRequestCompleted)
	<-isRequestCompleted
}

func Task32() {
	fmt.Println("Starting proxy server on port 8080...")
	http.HandleFunc("/", handleIncomingRequest)
	http.ListenAndServe(":8080", nil)
}
