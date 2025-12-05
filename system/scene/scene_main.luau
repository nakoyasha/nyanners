local imageLabel = game.ImageLabel

local label = Instance.new("TextLabel")
label.Text = "hi"
label.Parent = game

print("Scene initializing")

function engine_update()
    -- move the ichika hoshino
    local ichika = game.ImageLabel

    ichika.Position = vector.create(math.random(1, 200), math.random(1, 200))
end