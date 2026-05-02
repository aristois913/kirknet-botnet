package main

import "strings"

func Sanitize(input string) string {
	return strings.Map(func(r rune) rune {
		if r >= 32 && r <= 126 {
			return r
		}
		return -1
	}, input)
}
