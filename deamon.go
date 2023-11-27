package main

import (
	"fmt"
	"log"
	"os"
	"os/signal"
	"sync"
	"syscall"
	"time"
)

const (
	stopTimeout  = 5 * time.Second
	tickInterval = 2 * time.Second
)

var wg sync.WaitGroup

// run is responsible for the main logic of the daemon.
func run(stopCh <-chan struct{}) {
	defer wg.Done()

	// Use a ticker for periodic tasks.
	ticker := time.NewTicker(tickInterval)
	defer ticker.Stop()

	for {
		select {
		case <-stopCh:
			fmt.Println("Stopping daemon...")
			return
		case <-ticker.C:
			fmt.Println("Daemon is running...")
		}
	}
}

// setupSignalHandling sets up signal handling for graceful shutdown.
func setupSignalHandling(stopCh chan<- struct{}) {
	signalChannel := make(chan os.Signal, 1)
	signal.Notify(signalChannel, os.Interrupt, os.Kill, syscall.SIGTERM)

	go func() {
		sig := <-signalChannel
		log.Printf("Received signal %v. Stopping daemon...\n", sig)

		// Close the stop channel to signal the daemon to stop.
		close(stopCh)

		// Allow time for cleanup before exiting.
		time.Sleep(stopTimeout)

		// Exit the program.
		os.Exit(0)
	}()
}

// startDaemon initializes and starts the daemon.
func startDaemon() chan struct{} {
	stopCh := make(chan struct{})

	fmt.Println("Starting daemon...")

	// Increment the wait group to indicate the presence of a running goroutine.
	wg.Add(1)

	// Start the main logic of the daemon in a goroutine.
	go run(stopCh)

	// Set up signal handling for graceful shutdown.
	setupSignalHandling(stopCh)

	// Return the stop channel to allow external control over the daemon.
	return stopCh
}

func main() {
	// Start the daemon and receive the stop channel.
	stopCh := startDaemon()

	// Wait for the daemon to finish its execution.
	wg.Wait()

	// Print a message indicating that the daemon has stopped.
	fmt.Println("Daemon stopped.")

	// Ensure the stop channel is closed.
	close(stopCh)
}
