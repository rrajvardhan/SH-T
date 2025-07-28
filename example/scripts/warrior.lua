-- warrior.lua
local M = {}
local warrior = {
	id = nil,
	canJump = true,
	state = nil,
	stateTimer = 0,
}

-- FSM States
local states = {}

states.Idle = {
	enter = function()
		play_animation(warrior.id, "IDLE")
	end,

	update = function(dt)
		if Input:key_down(KEY_D) or Input:key_down(KEY_A) then
			return "Run"
		elseif Input:key_pressed(KEY_SPACE) and warrior.canJump then
			return "Jump"
		end
		if Input:key_pressed(KEY_J) then
			return "Attack"
		end
	end,
}

states.Run = {
	enter = function()
		play_animation(warrior.id, "RUN")
	end,

	update = function(dt)
		if not (Input:key_down(KEY_D) or Input:key_down(KEY_A)) then
			return "Idle"
		elseif Input:key_pressed(KEY_SPACE) and warrior.canJump then
			return "Jump"
		end
		if Input:key_pressed(KEY_J) then
			return "Attack"
		end
	end,
}

states.Jump = {
	enter = function()
		play_animation(warrior.id, "JUMP")
		local rb = get_rigidbody(warrior.id)
		rb.velocity.y = -600
		warrior.canJump = false
	end,

	update = function(dt)
		local rb = get_rigidbody(warrior.id)
		if rb.velocity.y > 0 then
			return "Fall"
		end
	end,
}

states.Fall = {
	enter = function()
		play_animation(warrior.id, "JUMP")
	end,

	update = function(dt)
		local rb = get_rigidbody(warrior.id)
		if rb.velocity.y == 0 and warrior.canJump then
			return "Idle"
		end
	end,
}

states.Attack = {
	enter = function()
		play_animation(warrior.id, "ATTACK")
		warrior.stateTimer = 0.4 -- duration of attack lock
		local rb = get_rigidbody(warrior.id)
		rb.velocity.x = 0
	end,

	update = function(dt)
		warrior.stateTimer = warrior.stateTimer - dt
		if warrior.stateTimer <= 0 then
			if Input:key_down(KEY_D) or Input:key_down(KEY_A) then
				return "Run"
			else
				return "Idle"
			end
		end
	end,
}

function M.init()
	warrior.id = find_entity_by_name("Warrior")
	warrior.state = states.Idle
	warrior.state.enter()

	subscribe_event("CollisionEvent", function(payload)
		if not warrior.id then
			return
		end

		local a, b = payload.entity_a, payload.entity_b
		if a == warrior.id or b == warrior.id then
			local other = (a == warrior.id) and b or a
			if get_identification(other).name == "Ground" then
				warrior.canJump = true
			end
		end
	end)
end

function M.update(dt)
	if not warrior.id then
		local found = find_entity_by_name("Warrior")
		if found then
			warrior.id = found
		else
			return
		end
	end

	local rb = get_rigidbody(warrior.id)

	-- Horizontal Movement (ignored if locked)
	if warrior.stateTimer <= 0 then
		if Input:key_down(KEY_D) then
			rb.velocity.x = 200
			set_sprite_flip(warrior.id, "none")
		elseif Input:key_down(KEY_A) then
			rb.velocity.x = -200
			set_sprite_flip(warrior.id, "horizontal")
		else
			rb.velocity.x = 0
		end
	end

	-- Update current state
	local nextState = warrior.state.update(dt)
	if warrior.stateTimer <= 0 and nextState and states[nextState] and states[nextState] ~= warrior.state then
		warrior.state = states[nextState]
		warrior.state.enter()
	end
end

return M
