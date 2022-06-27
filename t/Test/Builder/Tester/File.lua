
--
-- lua-TestMore : <http://fperrad.github.com/lua-TestMore/>
--

local tconcat = require 'table'.concat
local setmetatable = setmetatable

local tb = require 'Test.Builder'.new()

_ENV = nil
local m = {}

function m.new (_type)
    local o = setmetatable({ type = _type }, { __index = m })
    o:reset()
    return o
end

function m:write (...)
    sel