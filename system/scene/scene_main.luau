local label = Instance.new("TextLabel")
label.Text = "hi"
label.Parent = game

local ichika = Instance.new("ImageLabel")
ichika.Parent = game

print("Scene initializing")

local script = Instance.new("Script")
script.Name = "testScript"
script.Source = `print("oh hey, this is created here :3")`
script.Parent = game


game.Tick:Connect(function()
    -- move the ichika hoshino
    ichika.Position = vector.create(math.random(1, 200), math.random(1, 200))
end)