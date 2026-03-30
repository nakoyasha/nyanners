local IOService = DataModel:get_service("IOService")
local RunService = DataModel:get_service("RunService")
local RenderingService = DataModel:get_service("RenderingService")
local UIService = DataModel:get_service("UIService")

local label = UIService:find_first_child("TextLabel")

print(IOService:read_file("C:/Users/Haruka/Documents/mitmproxy.conf"))

RunService.PreRender:Connect(function()
    print(RenderingService.fps)
    label.Text = string.format("FPS: %d", RenderingService.fps)
end)

RunService.OnExit:Connect(function()
    print("we're exiting now")
end)