local IOService = DataModel:get_service("IOService")
local RunService = DataModel:get_service("RunService")
local RenderingService = DataModel:get_service("RenderingService")
local UIService = DataModel:get_service("UIService")
local World = DataModel:get_service("World")

local start = os.clock()

--local label = UIService:find_first_child("TextLabel")
--World.Active = false
-- local Ena = World.ena
local Ena = World.TextLabel

function tick()
    return os.clock() - start
end

RunService.PreRender:Connect(function()
    --label.Text = string.format("FPS: %d", RenderingService.fps)
    local y = 25 + math.sin(tick()) * 10

    if (y > 0) then
        Ena.Position = Vector3.new(0, y, 0)
    end

    Ena.Color = Color3.new(155, math.sin(tick()) * 255, 155, 255)
end)

RunService.OnExit:Connect(function()
    print("we're exiting now")
end)