package main

import (
	"log"

	"github.com/BurntSushi/toml"
)

var (
	Options = new(Config)
)

type Config struct {
	Kirknet struct {
		Server struct {
			Protocol string `toml:"protocol"`
			Port     string `toml:"port"`
		} `toml:"server"`
		Spreader struct {
			Protocol string `toml:"protocol"`
			Port     string `toml:"port"`
		} `toml:"spreader"`
	} `toml:"kirknet"`
}

func loadConfig(config interface{}, path string) {
	if _, err := toml.DecodeFile(path, config); err != nil {
		log.Fatalf("Error loading config: %s", err)
	}
}
