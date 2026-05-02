package main

import (
	"net"
	"sync"
)

var (
	bots  []net.Conn
	mutex sync.Mutex
)

func addBot(connection net.Conn) {
	mutex.Lock()
	defer mutex.Unlock()
	bots = append(bots, connection)
}

func removeBot(connection net.Conn) {
	mutex.Lock()
	defer mutex.Unlock()

	for i, c := range bots {
		if c == connection {
			bots = append(bots[:i], bots[i+1:]...)
			break
		}
	}
}

func botCount() int {
	mutex.Lock()
	defer mutex.Unlock()

	return len(bots)
}
