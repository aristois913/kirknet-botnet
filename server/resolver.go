package main

import dns "github.com/bogdanovich/dns_resolver"

var resolver *dns.DnsResolver = nil

func Resolver() {
	resolver = dns.New([]string{"1.1.1.1", "8.8.8.8"})
}
