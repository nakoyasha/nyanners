local label = Instance.new("TextLabel")
label.Text = "hi"
label.Parent = game

local ichika = Instance.new("ImageLabel")
ichika.Parent = game

print("Scene initializing")

game.Tick:Connect(function()
    -- move the ichika hoshino
    ichika.Position = vector.create(math.random(1, 200), math.random(1, 200))
end)