--# selene: allow(incorrect_standard_library_use)
engine = {
	updateListeners = {},
	version = "v0.0.1a",
}

function engine:connectToUpdate(callback)
	table.insert(engine.updateListeners, callback)
end

function engine:_internalUpdate()
	for _, listener in pairs(engine.updateListeners) do
		local routine = coroutine.create(listener)
		coroutine.resume(routine)
	end
end

function engine.quit()
	engine_DispatchNative("Engine.Exit")
end

coroutine.wrap(function()
	task = {}

	function task.wait(seconds)
		-- table.insert(tasksToResume, {
		-- coroutine = coroutine.running(),
		-- start = os.clock(),
		-- time = seconds,
		-- })
		return coroutine.yield(seconds)
	end
end)()
