--This file specifies the box controller.
--On initialization it creates a box of the parameters specified.

local box = nil
local float = true;
--Creates a box and sets the texture of the box to Data/Black.png.
function initialize(params)
	if params:size() == 4 then
		local position = params:get_vector(0);
		local size = params:get_vector(1);
		local mass = params:get_double(2);
		local velocity = params:get_vector(3);
		box = game:create_box(position, size, mass);
		box:load_texture("Data/Black.png");
		box.name = "box";
		box.bounce = 0.5;
		box.friction = 0.5;
		box.id = 1;
		box:set_velocity(velocity.x, velocity.y, velocity.z);

		game:set_collision_callback(box, "collision_callback");
		return true;
	else
		return false;
	end
end

function collision_callback(object)
	if object.id == 2 then
		game:remove_self();	
	end 
	return true;
end

function control()
	local gravity = game:get_int_property("gravity");	
	if gravity == 1 then
		box:add_force(0, 0, 9.8); --eliminate gravity
	end
	
	return true;
end

--This particular controller handles two messages sent to it.
--One is "levitate" which basically adds a certain force under the box to make it lift momentarily.
--Continuous levitate messages would obviously make the boxs float off into space.
--The box also accepts the destroy message. This is used by main.lua to destroy all boxes at once.
function handle_message(message, params, from)
	local string_message = message:get_string();
	
	if string_message == "levitate" then
		local force = params:get_vector(0);
		box:add_force(force.x, force.y, force.z);	
	elseif string_message == "destroy" then
		game:remove_self();
	else
	end
	
end

function destroy()	
	game:remove_object(box);	
	box = nil;	
end
