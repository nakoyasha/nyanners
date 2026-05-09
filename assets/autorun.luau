local IOService = DataModel:get_service("IOService")
local RunService = DataModel:get_service("RunService")
local RenderingService = DataModel:get_service("RenderingService")
local UIService = DataModel:get_service("UIService")
local World = DataModel:get_service("World")

local start = os.clock()
local label = UIService:find_first_child("TextLabel")

local blocks = {}

local function makeBlock(position)
	local mesh = Instance.new("MeshPart")
	mesh:set_model("assets/models/teapot.obj")

	mesh.Position = position
	mesh.Color = Color3.new(
		math.round(math.random(1, 255)),
		math.round(math.random(1, 255)),
		math.round(math.random(1, 255)),
		255
	)
	mesh.Parent = World

	table.insert(blocks, mesh)
end

function tick()
	return os.clock() - start
end

for x = 1, 16, 1 do
	for z = 1, 16, 1 do
		makeBlock(Vector3.new(x + 128, 0, z))
	end
end

RunService.PreRender:Connect(function()
	for i, block in blocks do
		block.Position = Vector3.new(block.Position.x, i + math.sin(tick()), block.Position.z)
	end
end)

RunService.OnExit:Connect(function()
	print("we're exiting now")
end)
