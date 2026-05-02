package main

import (
	"log"
	"net"
)

func Server() {
	listener, err := net.Listen(Options.Kirknet.Server.Protocol, Options.Kirknet.Server.Port)
	if err != nil {
		log.Fatalf("Error: %v\\r\\n", err)
	}

	log.Println("Started server on port: " + Options.Kirknet.Server.Port)
	go Resolver()

	for {
		connection, err := listener.Accept()
		if err != nil {
			continue
		}
		go Admin(connection)
	}
}
