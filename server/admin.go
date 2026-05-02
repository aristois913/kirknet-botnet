package main

import (
	"fmt"
	"net"
	"strings"
)

func Admin(connection net.Conn) {
	defer connection.Close()

	connection.Write([]byte("\033[?1049h\033[2J\033[1H"))
	connection.Write([]byte{255, 251, 1, 255, 251, 3, 255, 252, 34})

	connection.Write([]byte("\033]0;K1RKN3T . \007"))

	connection.Write([]byte(art))

	var history []string

	for {

		input, err := ReadWithHistory(connection, "\x1b[34mroot@kirknet\x1b[37m:~# ", "", 100, history)
		if err != nil {
			return
		}

		command := strings.ToLower(strings.TrimSpace(Sanitize(input)))
		if command == "" {
			continue
		}

		history = append(history, command)

		switch command {
		case "help":
			connection.Write([]byte("Available: help, cls, whoami, exit, echo\r\n"))
		case "cls", "clear":
			connection.Write([]byte("\033[2J\033[1H"))
		case "whoami":
			connection.Write([]byte("root\r\n"))
		case "exit", "quit":
			connection.Write([]byte("\033[?1049l"))
			return
		case "bots":
			connection.Write([]byte(fmt.Sprintf("Connected bots: %d\r\n", botCount())))
		default:
			if strings.HasPrefix(command, "echo ") {
				connection.Write([]byte(strings.TrimPrefix(command, "echo ") + "\r\n"))
			} else {
				connection.Write([]byte(fmt.Sprintf("-bash: %s: command not found\r\n", command)))
			}
		}
	}
}
