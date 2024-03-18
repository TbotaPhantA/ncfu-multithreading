// package main

// import (
// 	"fmt"
// 	"io"
// 	"net/http"
// )

// // Simple in-memory cache
// var cache map[string][]byte = make(map[string][]byte)

// func handleRequest(w http.ResponseWriter, r *http.Request) {
// 	target := "https://google.com/"

// 	// Check cache for existing response
// 	path := r.URL.String()
// 	if data, ok := cache[path]; ok {
// 		w.WriteHeader(http.StatusOK)
// 		w.Write(data)
// 		fmt.Println("Serving from cache:", path)
// 		return
// 	}

// 	// Not cached, fetch from Google
// 	client := &http.Client{}
// 	req, err := http.NewRequest(http.MethodGet, target + path, nil)
// 	if err != nil {
// 		http.Error(w, err.Error(), http.StatusInternalServerError)
// 		return
// 	}

// 	resp, err := client.Do(req)
// 	if err != nil {
// 		http.Error(w, err.Error(), http.StatusBadGateway)
// 		return
// 	}
// 	defer resp.Body.Close()

// 	// Read response body
// 	body, err := io.ReadAll(resp.Body)
// 	if err != nil {
// 		http.Error(w, err.Error(), http.StatusInternalServerError)
// 		return
// 	}

// 	// Cache the response
// 	cache[path] = body

	
// 	// Write response to client
// 	w.WriteHeader(resp.StatusCode)
// 	w.Write(body)
// 	fmt.Println("Fetched from Google:", path)
// }

// func Task31() {
// 	fmt.Println("Starting proxy server on port 8080...")
// 	http.HandleFunc("/", handleRequest)
// 	http.ListenAndServe(":8080", nil)
// }
