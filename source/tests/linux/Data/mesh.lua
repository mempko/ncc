--This file specifies the car controller.
--On initialization it creates a car of the parameters specified.


local car = nil
local float = true;
--Creates a car and sets the texture of the car to Data/Black.png.
function initialize(params)
	if params:size() == 4 then
		local position = params:get_vector(0);
		local size = params:get_vector(1);
		local mass = params:get_double(2);
		local velocity = params:get_vector(3);
		car = game:create_mesh("Data/truck.obj", position, mass);
	    car:load_texture("Data/car.jpg");
		car.name = "car";
		car.bounce = 0.5;
		car.friction = 0.5;
		car.id = 1;
		car:set_velocity(velocity.x, velocity.y, velocity.z);

		--game:set_collision_callback(car, "collision_callback");
		return true;
	else
		return false;
	end
end

function collision_callback(object)
	return true;
end

--Currently there is no logic for the car
function control()
	return true;
end

--This particular controller handles two messages sent to it.
--One is "levitate" which basically adds a certain force under the car to make it lift momentarily.
--Continous levitate messages would obviously make the cars float off into space.
--The car also accepts the destroy message. This is used by main.lua to destroy all cares at once.
function handle_message(message, params, from)
	local string_message = message:get_string();
	if string_message == "levitate" then
		local force = params:get_vector(0);
		car:add_force(force.x, force.y, force.z);	
	end
	
end

function destroy()	
	game:remove_object(car);	
	car = nil;	
end
