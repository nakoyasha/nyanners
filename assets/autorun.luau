local IOService = DataModel:get_service("IOService")
local RunService = DataModel:get_service("RunService")
local RenderingService = DataModel:get_service("RenderingService")
local UIService = DataModel:get_service("UIService")
local World = DataModel:get_service("World")

local start = os.clock()
local label = UIService:find_first_child("TextLabel")

local mainCamera = RenderingService.MainCamera
local secondaryCamera = RenderingService.SecondaryCamera

local blocks = {}
local cameraModels = {}

-- label.Text = string.rep("hi", 99999)
label.Text = [[
Lorem ipsum dolor sit amet, consectetur adipiscing elit. Duis egestas leo elit, non posuere nisi tempus sed. Ut sed dolor gravida, rutrum felis ac, finibus odio. Donec tincidunt tristique ligula eu finibus. Nulla pretium eros tincidunt tristique consectetur. Donec varius convallis ligula, vel tempor tortor eleifend et. Suspendisse dictum tortor id erat cursus, sit amet scelerisque lacus venenatis. Pellentesque non nunc suscipit, dignissim purus ut, dictum felis.

Cras tincidunt orci elit, in consequat odio efficitur ut. Pellentesque nec sodales dolor, vitae mattis purus. Proin scelerisque lorem est, id blandit orci fermentum eu. Nam sed erat sapien. Quisque mi massa, venenatis vel neque quis, rhoncus pretium nunc. Duis non magna est. Sed porta gravida pretium. Nullam neque risus, dapibus sit amet malesuada at, porta nec urna. Proin cursus neque vel blandit mollis.

Quisque velit risus, pretium vel vestibulum eu, congue quis ex. Nam scelerisque tempor metus, nec feugiat diam auctor in. Nullam fermentum suscipit purus ac porta. Praesent eget nisl fermentum, blandit urna ac, pulvinar dui. Pellentesque hendrerit quis ipsum eu gravida. Integer non elit ex. Etiam vel mi id enim rhoncus elementum id id sapien. Aliquam sit amet luctus est, sit amet scelerisque odio. Nam rhoncus blandit mattis.

Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam nisi augue, semper at accumsan eget, mattis vel sapien. Donec ullamcorper eleifend urna, eget consectetur nibh luctus at. Morbi faucibus fringilla turpis vitae ornare. Donec pulvinar id orci in imperdiet. Maecenas finibus nibh sed ante fringilla tincidunt. Sed bibendum feugiat odio, sit amet maximus lacus faucibus non. Nam rhoncus sed elit id sagittis. Vivamus id ultrices mi. Integer et tempor magna, ut maximus ante. Quisque tellus nibh, finibus sit amet dapibus eu, hendrerit in arcu. Mauris turpis nisl, dictum non augue et, dictum venenatis metus.

Ut feugiat nulla mattis, congue sapien in, mollis metus. Praesent eleifend arcu in libero semper tristique. Integer nisi arcu, luctus non metus vitae, venenatis dictum tellus. Sed sollicitudin massa ligula, gravida congue dui pulvinar quis. Vestibulum tincidunt purus sit amet neque fringilla lobortis. Nullam dictum risus sed velit malesuada luctus. Mauris lectus nunc, euismod at lorem id, porta congue mauris. Sed hendrerit sodales lectus quis porttitor. Aliquam nec leo rhoncus, varius elit ut, bibendum ipsum. Proin cursus euismod erat, eget euismod lectus sagittis et. Nunc eget elit leo. Integer orci nisl, ornare at dictum id, tempus sit amet ligula. Duis egestas at leo eget imperdiet.

Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Etiam ut fringilla nisi. Nam urna enim, pretium ut euismod in, finibus eget velit. In non dui nisl. In hac habitasse platea dictumst. Quisque molestie leo ac lacus dignissim laoreet. Morbi a pretium nisi.

Mauris blandit, mi malesuada lacinia tempus, justo dui dignissim magna, ac pellentesque ligula ligula et ante. Nulla eget ex ac elit rutrum faucibus. Sed ante velit, commodo quis egestas sed, egestas sit amet justo. Curabitur laoreet eros non risus euismod posuere. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Sed dapibus augue in maximus bibendum. Nulla vehicula sagittis tortor, eu hendrerit erat porttitor a. Integer gravida rhoncus purus a elementum. Praesent congue arcu sed justo rutrum tempus. Phasellus interdum nec metus et mattis. Ut fringilla nisi vel ipsum tincidunt, eget rhoncus ipsum tempus. Donec vehicula sollicitudin nibh, in condimentum ex porttitor et. Nulla purus nisl, lacinia a pretium ornare, efficitur vitae augue.

In eros metus, feugiat non velit eu, dignissim blandit nunc. Donec consectetur accumsan risus, ac sodales ante ultrices eget. Aliquam erat volutpat. Quisque volutpat sapien ut aliquet vulputate. Morbi ut cursus elit. Maecenas quis mollis quam, eget iaculis dui. Fusce et tortor ultrices, imperdiet elit et, lobortis mauris. Sed aliquam aliquet ex. Duis id porttitor diam. Nulla facilisi. Cras dapibus neque nulla, sit amet facilisis est ultrices scelerisque. Vivamus sagittis arcu diam, id semper mi tincidunt nec. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Ut vitae tincidunt metus. Aenean tempus massa eu hendrerit molestie. Vivamus vitae mi tristique, vulputate nibh vel, lacinia ante.
]]

local function makeCameraModel(camera)
	local model = Instance.new("MeshPart")
	model:set_model("assets/models/camera.obj")
	model.Position = camera.Position
	model.Parent = World
	model.Scale = Vector3.new(0.01, 0.01, 0.01)

	table.insert(cameraModels, { camera = camera, model = model })
end

makeCameraModel(mainCamera)
makeCameraModel(secondaryCamera)

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

--for x = 1, 16, 1 do
--	for z = 1, 16, 1 do
--		makeBlock(Vector3.new(x + 128, 0, z))
--	end
--end

-- makeBlock(Vector3.new(64, 64, 64).Parent = DataModel.World)

-- local newVector = newproxy(true)
-- local meta = getmetatable(newVector)

-- meta["__index"] = function(self, k)
-- 	if k == "x" then
-- 		return 0
-- 	elseif k == "y" then
-- 		return 0.5
-- 	elseif k == "z" then
-- 		return 0.9
-- 	else
-- 		return 0
-- 	end
-- end

-- meta["__tostring"] = function()
-- 	return "hi"
-- end

RunService.PreRender:Connect(function()
	for _, m in cameraModels do
		-- todo: add math support to Vector3
		m.model.Position = Vector3.new(m.camera.Position.x, m.camera.Position.y, m.camera.Position.z)
			- Vector3.new(0, 0, 0.5)
		m.model.Rotation = m.camera.Rotation
	end
end)

RunService.OnExit:Connect(function()
	print("we're exiting now")
end)
