package main

import (
	"fmt"
	"net/http"
)

func handler(w http.ResponseWriter, r *http.Request) {
  fmt.Fprintf(w, "Hello, world")
}

func main() {
  http.HandleFunc("/", handler)
  fmt.Println("Server listening on port localhost:8080")
  http.ListenAndServe(":8080", nil)
}
