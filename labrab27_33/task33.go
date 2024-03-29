package main

import (
	"flag"
	"fmt"
	"io"
	"net/http"
	"strconv"
	"sync"
)

// Simple in-memory cache
var cacheTask33 map[string][]byte = make(map[string][]byte)

func handleRequestTask33(w http.ResponseWriter, r *http.Request, done chan struct{}) {
	target := "https://google.com/"

	// Check cache for existing response
	path := r.URL.String()
	if data, ok := cacheTask33[path]; ok {
		w.WriteHeader(http.StatusOK)
		w.Write(data)
		fmt.Println("Serving from cache:", path)
		defer func() {
			close(done) // Signal completion
		}()
		return
	}

	// Not cached, fetch from Google
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

	// Read response body
	body, err := io.ReadAll(resp.Body)
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
		return
	}

	// Cache the response
	cacheTask33[path] = body

	// Write response to client
	w.WriteHeader(resp.StatusCode)
	w.Write(body)
	fmt.Println("Fetched from Google:", path)
	defer func() {
		close(done) // Signal completion
	}()
}

var (
	threadsStr string
)

func Task33() {
	fmt.Println("Starting proxy server on port 8080...")
	flag.StringVar(&threadsStr, "threads", "4", "number of threads")

	threads, err := strconv.Atoi(threadsStr)
	if err != nil {
		panic(err)
	}

	var wg sync.WaitGroup

	// Create a pool of 4 workers
	workerPool := make(chan struct{}, threads)

	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		workerPool <- struct{}{} // Acquire a worker slot
		wg.Add(1)

		doneUpper := make(chan struct{})
		go func(doneUpper chan struct{}) {
			defer func() {
				<-workerPool // Release the slot when done
				wg.Done()
			}()

			done := make(chan struct{})
			go handleRequestTask33(w, r, done) // Delegate work to the worker
			<-done
			defer func() {
				close(doneUpper) // Signal completion
			}()
		}(doneUpper)
		<-doneUpper
	})

	http.ListenAndServe(":8080", nil)
	wg.Wait() // Ensure all workers finish before exiting
}
