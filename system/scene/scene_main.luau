local characters = require("./characters.luau")
local workspace = game:getService("Workspace")
local scriptService = game:getService("ScriptService")

game:SetWindowTitle("hi")

local ichika = Instance.new("ImageLabel")
ichika.Name = "evil"
ichika.Parent = workspace

local background = Instance.new("ImageLabel") :: ImageLabel
background.Texture = "assets/Ena_10_trained_art.png"
background.Size = vector.create(1, 1)
background.Name = "background"
background.Parent = workspace

print("Scene initializing")

local script = Instance.new("Script")
script.Name = "testScript"
script.Source = `print("oh hey, this is created here :3")`
script.Parent = scriptService

local sound = Instance.new("Sound") :: Sound
sound.Name = "crazy"
sound.Sound = "assets/crazy.mp3"

local label = Instance.new("TextLabel")
label.Text = "FPS:"
label.Parent = workspace

-- sound:Play()
sound.Parent = workspace

game.Tick:Connect(function(something)
	-- move the ichika hoshino
    -- ichika.Position = vector.create(math.random(1, 200), math.random(1, 200))
	-- update fps
	label.Text = `FPS: {game:GetFPS()}`
end)
