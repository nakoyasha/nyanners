function engine_update() end

print("Our name is", tostring(script))
print("My parent is", script.Parent.Name)

print("Game is", game.Name)
print("trying to set fps cap to 120")
game:setFPS(120)

print("trying to set our name")
script.Name = "hi"
print(script.Name)
print("let's make a http request!")

-- create http
local http = game:getService("HttpService")
local response = http:request("jsonplaceholder.typicode.com", "/todos/1")
print(response.code, response.body)
print(typeof(response.body))
print(http.Name)
local json = http:JSONDecode(response.body)

for i, v in json do
	print(i, v)
end
