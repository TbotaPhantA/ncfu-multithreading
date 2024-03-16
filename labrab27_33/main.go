package main

import (
	"fmt"
	"io"
	"net/http"
)

func handler(w http.ResponseWriter, r *http.Request) {
  fmt.Fprintf(w, "Hello, world (from origin server)")
}

func proxyHandler(w http.ResponseWriter, r *http.Request) {
  // Forward request to origin server
  originResponse, err := http.Get("http://localhost:8080" + r.URL.Path)
  if err != nil {
    fmt.Fprintf(w, "Error forwarding request: %v", err)
    return
  }
  defer originResponse.Body.Close()

  // Copy response headers
  for key, values := range originResponse.Header {
    for _, value := range values {
      w.Header().Set(key, value)
    }
  }

  // Copy response body
  _, err = io.Copy(w, originResponse.Body)
  if err != nil {
    fmt.Fprintf(w, "Error copying response body: %v", err)
    return
  }
}

func main() {
  // Create separate instances of http.ServeMux
  originMux := http.NewServeMux()
  proxyMux := http.NewServeMux()

  // Register handlers on separate muxes
  originMux.HandleFunc("/", handler)
  proxyMux.HandleFunc("/", proxyHandler)

  // Start servers using respective muxes
  go http.ListenAndServe(":8080", originMux)
  fmt.Println("Origin server listening on port localhost:8080")
  fmt.Println("Proxy server listening on port localhost:8081")
  http.ListenAndServe(":8081", proxyMux)
}
