
#!/usr/bin/env lua
local socket = require "ssocket"

local paths = {}
local contents = {}

for i = 1, 20 do
  local path = string.format('/archives/game/%05d.html', i)
  table.insert(paths, path)