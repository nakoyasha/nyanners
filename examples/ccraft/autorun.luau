local RenderingService = DataModel:get_service("RenderingService")
local InputService = DataModel:get_service("InputService")
RenderingService:set_window_title("trash computercraft")

local LayerCollector = Instance.new("LayerCollector")
LayerCollector.Parent = DataModel.UIService

local lines = {}
local labels = {}

function line_add(text : string)
	table.insert(lines, text)

	if #lines > 10 then
		table.remove(lines, 1)
	end
end

local number = 0
InputService.OnInput:Connect(function(input : InputObject)
	if (input.InputState ~= Enum.InputState.Began) then
		return;
	end

	if (input.KeyCode == Enum.KeyCode.A) then
		line_add("hii ".. number)
		number += 1
	end
end)

DataModel.RunService.Tick:Connect(function()
	for _, child in labels do
		child:destroy()
	end
	table.clear(labels)

	local lastPosition = Vector2.new(0, 0, 0);

	for _, line in lines do
		local label = Instance.new("TextLabel")
		label.Text = line

		label.PositionOffset = lastPosition
		label.Parent = LayerCollector
		lastPosition = Vector2.new(0, lastPosition.y + 48 * 1.2)

		table.insert(labels, label)
	end
end)