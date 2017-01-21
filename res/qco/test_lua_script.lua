
local TestLua = class("TestLua")

TestLua.EditorProperties = {
	{"id", "int"},
	{"title", "string"},
	{"icon", "Image"},
	{"addr", "string"},
}

function TestLua:ctor()
	print("TestLua:ctor", self)
end

function TestLua:onBind()
	print("TestLua:onBind", self)
end

function TestLua:onUnbind()
	print("TestLua:onUnbind", self)
end

function TestLua:onAdd()
	print("TestLua:onAdd", self)
end

function TestLua:onRemove()
	print("TestLua:onRemove", self)
end

return TestLua
