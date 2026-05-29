local UIService = DataModel.UIService
local RunService = DataModel.RunService
local RenderingService = DataModel:get_service("RenderingService")

local Label = Instance.new("TextLabel")
local Button = Instance.new("Button")
local AspectRatio = Instance.new("UIAspectRatioComponent")
local LayerCollector = Instance.new("LayerCollector")
local Frame = Instance.new("Frame")

RenderingService:set_window_title("world's shittiest cookie clicker")

-- AspectRatio.AspectRatio = 1
Frame.Parent = LayerCollector
Frame.SizeScale = Vector2.new(1,1)
Frame:set_texture("assets/textures/ichika.png")
Label.Parent = Frame
Button.Parent = Frame
AspectRatio.Parent = Button
LayerCollector.Parent = UIService

local cookieCount = 0

Button.SizeScale = Vector2.new(0.5, 0.5)
Button.PositionScale = Vector2.new(0.150, 0.5)
Button.AnchorPoint = Vector2.new(-0.5, -0.5)

Button.Color = Color3.new(255, 255, 255, 255)
Button:set_texture("assets/textures/cookie.png")

Button.Held:Connect(function()
    Button.SizeScale = Vector2.new(0.35, 0.35)
end)

Button.Released:Connect(function()
    Button.SizeScale = Vector2.new(0.5, 0.5)
end)

Button.Activated:Connect(function()
    print("omg im pressed... revolutionary...")
    cookieCount += 1
end)

RunService.Tick:Connect(function()
   -- local window_size = DataModel.RenderingService.ViewportSize
   -- Label.Position = Vector3.new(0, window_size.y - 48, 0)
    Label.Text = string.format("%d cookies", cookieCount)
end)

-- print(Label.Name)
--Label.Name = "hi"
--DataModel.Name = "hi"
print(DataModel.Name)