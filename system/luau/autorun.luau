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
local response = http:request("https://jsonplaceholder.typicode.com/todos/1")
print(response.code, response.body)
print(typeof(response.body))
print(http.Name)
local json2 = http:JSONDecode('{"a": [1, 2, 3]}')
local json = http:JSONDecode(response.body)
print(json2)

for i, v in json2 do
	if type(v) == "table" then
		for i, v in v do
			print(i, v)
		end
	end
end
