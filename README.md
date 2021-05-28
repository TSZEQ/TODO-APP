
# Lua simple socket module

A simple socket module for lua. It supports: Lua 5.2+.

[![Build Status](https://travis-ci.org/cofyc/lua-ssocket.png?branch=master)](https://travis-ci.org/cofyc/lua-ssocket)

## Examples

```
socket = require "ssocket"
tcpsock = socket.tcp()
ok, err = tcpsock:connect("www.google.com", 80)
if err then
    print(err)
end
tcpsock:write("GET / HTTP/1.1\r\n")
tcpsock:write("\r\n")
tcpsock:settimeout(1)
data, err, partial = tcpsock:read(1024)
if err == socket.ERROR_TIMEOUT then
  data = partial
end
io.output():write(data)
tcpsock:close()
```

More examples, see *examples/* folder.