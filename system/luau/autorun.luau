function testString(a: string)
	print(a)
end

print("this is running from luau!")
testString("hii, it's typed!")

local tick = 0

function engine_update()
	tick += 1
	print(tick)

	-- if tick >= 100 then
	-- print(engine.quit())
	-- end

	engine_DrawText("Hii!")
	-- print("will we halt the thread")
end

-- while true do
-- print("h")
-- end

-- engine_DispatchNative("Engine.Exit")

-- engine_panic(string.rep("a", 1000))
