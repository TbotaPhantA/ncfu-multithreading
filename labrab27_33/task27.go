package main

import (
	"flag"
	"fmt"
	"io"
	"net/http"
)

func handler(w http.ResponseWriter, r *http.Request) {
  fmt.Fprintf(w, "Hello, world (from origin server)")
}

func proxyHandler(w http.ResponseWriter, r *http.Request) {
  // Forward request to origin server
	originResponse, err := http.Get("http://" + origin + ":" + originPort + r.URL.Path)
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

var (
    origin string
		originPort string
    proxyPort string
)

func Task27() {
	flag.StringVar(&origin, "origin", "localhost", "Address of the origin server")
	flag.StringVar(&originPort, "originPort", "8080", "Port for the origin server")
	flag.StringVar(&proxyPort, "proxyPort", "8081", "Port for the proxy server")
	flag.Parse()

  // Create separate instances of http.ServeMux
  originMux := http.NewServeMux()
  proxyMux := http.NewServeMux()

  // Register handlers on separate muxes
  originMux.HandleFunc("/", handler)
  proxyMux.HandleFunc("/", proxyHandler)

  // Start servers using respective muxes
  go http.ListenAndServe(":" + originPort, originMux)
  fmt.Println("Origin server listening on " + origin + originPort)
  fmt.Println("Proxy server listening on " + origin + proxyPort)
  http.ListenAndServe(":" + proxyPort, proxyMux)
}
