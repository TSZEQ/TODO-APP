#!/usr/bin/env lua
local socket = require "ssocket"

SOCK_PATH = '/tmp/test.sock'
tcpsock = socket.tcp()
os.remove(SOCK_PATH)
local ok, err = tcpsock:bind(SOCK_PATH)
if err then
  print(err)
  os.exit()
end
tcpsock:liste