#!/usr/bin/env lua
local socket = require "ssocket"

SOCK_PATH = '/tmp/test.sock'
tcpsock = socket.tcp()
os.remo