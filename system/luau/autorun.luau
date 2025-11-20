local tick = 0

function engine_update()
	tick += 1
	-- print("Tick", tick)
	-- -- engine_DrawText(script.Name)

	if tick == 100 then
		-- print("killing ourselves")
		-- script:Destroy()
	end

	-- if tick >= 600 then
	-- 	print("shouldn't be able to execute here")
	-- end
end

-- script.Name = "hi"
print("Our name is", tostring(script))
print("My parent is", script.Parent.Name)

print("Game is", game.Name)
print("trying to set fps cap to 120")
game:setFPS(120)

print("trying to set our name")
script.Name = "hi"
print(script.Name)
print("let's make a http request!")
local http = game:getService("HttpService")
local response = http:request("https://google.com")
print(response)
-- script.Name = nil

-- while true do
-- print("h")
-- end

-- engine_DispatchNative("Engine.Exit")

-- engine_panic(string.rep("a", 10000))
