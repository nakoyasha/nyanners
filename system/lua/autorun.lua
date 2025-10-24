-- require("system_global")

-- local text = {
-- "hii",
-- "hiiii",
-- "hiiiiii",
-- }
for i, v in pairs(system) do
	print(i, v)
end

engine:connectToUpdate(function()
	-- local randomText = text[math.random(1, #text)]
	engine_DrawText("hii")
end)

-- coroutine.wrap(function()
--     task.wait(5)
--     engine_panic("i don't like you. goodbye")

-- end)
