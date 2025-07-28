-- main.lua

package.loaded["warrior"] = nil
local warrior = require("warrior")

warrior.init()

function update()
	local dt = Timer:delta_time()
	warrior.update(dt)
end