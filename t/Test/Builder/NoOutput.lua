
--
-- lua-TestMore : <http://fperrad.github.com/lua-TestMore/>
--

local io = require 'io'
local pairs = pairs
local require = require

_ENV = nil
local m = {}

function m.create ()
    local tb = require 'Test.Builder'.create()
    tb:output(io.tmpfile())
    tb:failure_output(i