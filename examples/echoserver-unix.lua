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
tcpsock:listen(5)

addr, err = tcpsock:getsockname()
print(string.format("Listening on %s...", addr))
print(string.format("You can use this command to connect on: socat - UNIX-CONNECT:%s", SOCK_PATH))
print("")
while true do
  conn, err = tcpsock:accept()
