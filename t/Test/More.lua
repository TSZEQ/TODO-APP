

--
-- lua-TestMore : <http://fperrad.github.com/lua-TestMore/>
--

local loadstring = loadstring or load
local pairs = pairs
local pcall = pcall
local require = require
local tostring = tostring
local type = type
local unpack = require 'table'.unpack or unpack
local _G = _G

local tb = require 'Test.Builder'.new()

_ENV = nil
local m = {}

function m.plan (arg)
    tb:plan(arg)
end

function m.done_testing (num_tests)
    tb:done_testing(num_tests)
end

function m.skip_all (reason)
    tb:skip_all(reason)
end

function m.BAIL_OUT (reason)
    tb:BAIL_OUT(reason)
end

function m.ok (test, name)
    tb:ok(test, name)
end

function m.nok (test, name)
    tb:ok(not test, name)
end

function m.is (got, expected, name)
    local pass = got == expected
    tb:ok(pass, name)
    if not pass then
        tb:diag("         got: " .. tostring(got)
           .. "\n    expected: " .. tostring(expected))
    end
end

function m.isnt (got, expected, name)
    local pass = got ~= expected
    tb:ok(pass, name)
    if not pass then
        tb:diag("         got: " .. tostring(got)
           .. "\n    expected: anything else")
    end