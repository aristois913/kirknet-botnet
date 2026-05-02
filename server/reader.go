package main

import (
	"fmt"
	"net"
	"strings"
)

func Read(conn net.Conn, prompt, blocked string, maximumLen int) (string, error) {
	return read(conn, prompt, blocked, maximumLen, make([]string, 0))
}

func ReadWithHistory(conn net.Conn, prompt, blocked string, maximumLen int, history []string) (string, error) {
	return read(conn, prompt, blocked, maximumLen, history)
}

func read(conn net.Conn, prompt, blocked string, maximumLen int, history []string) (string, error) {
	if _, err := conn.Write([]byte(prompt)); err != nil {
		return "", err
	}

	var message []string = make([]string, 0)

	pos := len(history)

	for {
		var buf []byte = make([]byte, 1)
		_, err := conn.Read(buf)
		if err != nil {
			return "", err
		}

		switch buf[len(buf)-1] {
		case 16, 3, 2, 1, 11, 12, 5, 8, 31, 255, 251, 39, 24, 253:
			continue

		case 127:
			if len(message) <= 0 {
				continue
			}

			message = message[:len(message)-1]
			if _, err := conn.Write([]byte{127}); err != nil {
				return "", err
			}

		case 10, 13:
			if len(message) <= 0 {
				return "", nil
			}

			conn.Write([]byte("\r\n"))
			return strings.Join(message, ""), nil

		case 27:
			var buffer []byte = make([]byte, 5)
			if _, err := conn.Read(buffer); err != nil {
				return "", err
			}

			switch buffer[1] {
			case 65:
				if pos <= 0 {
					continue
				}

				pos--
				if _, err := conn.Write([]byte(fmt.Sprintf("\r\x1b[K%s%s", prompt, history[pos]))); err != nil {
					return "", err
				}

				message = strings.Split(history[pos], "")

			case 66:
				if pos+1 >= len(history) {
					continue
				}

				pos++
				if _, err := conn.Write([]byte(fmt.Sprintf("\r\x1b[K%s%s", prompt, history[pos]))); err != nil {
					return "", err
				}

				message = strings.Split(history[pos], "")
			}

		default:
			if len(message)+1 > maximumLen {
				continue
			}

			var write string = string(buf[0])
			if len(blocked) > 0 {
				write = blocked
			}

			if _, err := conn.Write([]byte(fmt.Sprint(write))); err != nil {
				return "", err
			}

			message = append(message, string(buf[0]))
		}
	}
}
