local IOService = DataModel:get_service("IOService")
local RunService = DataModel:get_service("RunService")
local RenderingService = DataModel:get_service("RenderingService")
local UIService = DataModel:get_service("UIService")
local World = DataModel:get_service("World")

--local label = UIService:find_first_child("TextLabel")
--World.Active = false

RunService.PreRender:Connect(function()
    --label.Text = string.format("FPS: %d", RenderingService.fps)
end)

RunService.OnExit:Connect(function()
    print("we're exiting now")
end)