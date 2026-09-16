local RenderingService = DataModel:get_service("RenderingService")
local InputService = DataModel:get_service("InputService")

local LayerCollector = DataModel:get_service("UIService").LayerCollector

local filesystem = {
	["/"] = {
		["hi.txt"] = "hii",
		["hi1.txt"] = "hii",
		["hi2.txt"] = "hii",
		["hi3.txt"] = "hii",
		["hi4.txt"] = "hii",
		["hi5.txt"] = "hii",
		["hi6.txt"] = "hii",
		["hi7.txt"] = "hii",
		["hi8.txt"] = "hii",
		["hi9.txt"] = "hii",
		["hi10.txt"] = "hii",
		["hi11.txt"] = "hii",
		["hi12.txt"] = "hii",
		["hi13.txt"] = "hii",
		["hi14.txt"] = "hii",
		["hi15.txt"] = "hii",
		["hi16.txt"] = "hii",
		["hi17.txt"] = "hii",
		["hhhhh"] = "new",
	},
}
local MAX_LINE_COUNT = 10
local TEXT_SIZE = 48

local currentLocation = "/"
local InputBox = LayerCollector.InputBox

local function get_current_location()
	return string.format("[%s]: ", currentLocation)
end

local lines = {}
local labels = {}

function line_add(text: string)
	table.insert(lines, text)

	local label = Instance.new("TextLabel")
	label.Text = text
	label.TextSize = TEXT_SIZE
	label.SizeScale = Vector2.new(1, 0.1);
	label.Parent = LayerCollector
	label.Name = #lines

	if #lines > MAX_LINE_COUNT then
		table.remove(lines, 1)
		label:destroy()
	else
		table.insert(labels, label)
	end

	local currentPosition = Vector2.new(0, 0)
	for index, label in labels do
		label.PositionOffset = currentPosition
		label.Text = lines[index]
		currentPosition = Vector2.new(currentPosition.x, currentPosition.y + TEXT_SIZE)
	end

	InputBox.PositionOffset = Vector2.new(currentPosition.x, currentPosition.y + TEXT_SIZE)
end

local keyInputs = {
	[Enum.KeyCode.A] = "a",
	[Enum.KeyCode.B] = "b",
	[Enum.KeyCode.C] = "c",
	[Enum.KeyCode.D] = "d",
	[Enum.KeyCode.E] = "e",
	[Enum.KeyCode.F] = "f",
	[Enum.KeyCode.G] = "g",
	[Enum.KeyCode.H] = "h",
	[Enum.KeyCode.I] = "i",
	[Enum.KeyCode.J] = "j",
	[Enum.KeyCode.K] = "k",
	[Enum.KeyCode.L] = "l",
	[Enum.KeyCode.M] = "m",
	[Enum.KeyCode.N] = "n",
	[Enum.KeyCode.O] = "o",
	[Enum.KeyCode.P] = "p",
	[Enum.KeyCode.Q] = "q",
	[Enum.KeyCode.R] = "r",
	[Enum.KeyCode.S] = "s",
	[Enum.KeyCode.T] = "t",
	[Enum.KeyCode.U] = "u",
	[Enum.KeyCode.V] = "v",
	[Enum.KeyCode.W] = "w",
	[Enum.KeyCode.X] = "x",
	[Enum.KeyCode.Y] = "y",
	[Enum.KeyCode.Z] = "z",

	[Enum.KeyCode.Zero] = "0",
	[Enum.KeyCode.One] = "1",
	[Enum.KeyCode.Two] = "2",
	[Enum.KeyCode.Three] = "3",
	[Enum.KeyCode.Four] = "4",
	[Enum.KeyCode.Five] = "5",
	[Enum.KeyCode.Six] = "6",
	[Enum.KeyCode.Seven] = "7",
	[Enum.KeyCode.Eight] = "8",
	[Enum.KeyCode.Nine] = "9",

	[Enum.KeyCode.Space] = " ",
	-- [Enum.KeyCode.Minus] = "-",
	-- [Enum.KeyCode.Equals] = "=",
	-- [Enum.KeyCode.LeftBracket] = "[",
	-- [Enum.KeyCode.RightBracket] = "[",
	-- [Enum.KeyCode.BackSlash] = "\\",
	-- [Enum.KeyCode.Semicolon] = ";",
	-- [Enum.KeyCode.Quote] = '"',
	-- [Enum.KeyCode.Comma] = "'",
	-- [Enum.KeyCode.Period] = ".",
	-- [Enum.KeyCode.Slash] = "/",
	-- [Enum.KeyCode.Backquote] = "~",
}

local timeSinceLastTick = os.clock()
local text = ""
local hasTick = false

-- TODO: this really needs require lmao
local commands = {
	["ls"] = function()
		local files = filesystem[currentLocation]

		local lineCountBeforeBreak = 0
		local output = ""

		for file, _ in files do
			if lineCountBeforeBreak > 3 then
				line_add(output)
				output = ""
			end

			output ..= file .. " "
			lineCountBeforeBreak += 1
		end

		line_add(output)
	end,
}

local function process_command(text: string)
	-- local args = text:split(" ")
	-- local commandName = args[0]

	local args = {}
	local command = commands[text]

	if command ~= nil then
		table.remove(args, 1)
		command(args)
	else
		line_add("Invalid command.")
	end
end

-- local time = 0
-- local next = os.clock() + 1 / 60

local radius = 16
local height = 5

DataModel.RunService.OnRender:Connect(function()
    -- crtShader:set_float("iTime", time)
  --  fireShader:set_float("iTime", time)
  	local time = os.clock() * 8

 	local lightPos = Vector3.new(
        math.cos(time) * radius,
        height,
        math.sin(time) * radius
    )

 	-- light.Position = lightPos

    -- local now = os.clock()
    -- if now >= next then
       -- time += 1 / 120
        -- next += 1 / 120
    --- end
end)

DataModel.RunService.Tick:Connect(function()
	-- print(os.clock() - timeSinceLastTick)
	if os.clock() - timeSinceLastTick >= 1 then
		if hasTick then
			InputBox.Text = get_current_location() .. text
			hasTick = false
			timeSinceLastTick = os.clock()
			return
		end

		InputBox.Text = get_current_location() .. text .. "_"
		hasTick = true
	end
end)

InputService.OnInput:Connect(function(input: InputObject)

	if input.InputState ~= Enum.InputState.Began then
		return
	end

	local key = keyInputs[input.KeyCode]

	if key then
		text ..= key
	end

	if input.KeyCode == Enum.KeyCode.Backspace then
		local lastCharacter = string.sub(text, 1, #text - 1)
		text = lastCharacter
	end

	InputBox.Text = string.format("[%s]: %s", currentLocation, text)

	if input.KeyCode == Enum.KeyCode.Enter then
		line_add(InputBox.Text)
		print(text)
		process_command(text)
		text = ""
		InputBox.Text = string.format("[%s]: %s", currentLocation, text)
	end
end)
