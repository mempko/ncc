
local sphere = nil
local float = true;
function initialize(params)
	if params:size() == 3 then
		local position = params:get_vector(0);
		local radius = params:get_double(1);
		local mass = params:get_double(2);
		sphere = game:create_sphere(position, radius, mass);
		sphere:load_texture("Data/Green.png");
		sphere.name = "sphere";
		sphere.bounce = 0.3;
		sphere.id = 2;
		game:set_collision_callback(sphere, "collision_callback");
		return true;
	else
		return false;
	end
end

function collision_callback(object)
	if object.id == 1 then
		game:remove_self();
	end 
	return true;
end

function control()
	
	local gravity = game:get_int_property("gravity");	
	if gravity == 1 then
		sphere:add_force(0, 0, 9.8); --eliminate gravity
	end
	
	local player = game:get_vector_property("player");
	local position = sphere:get_position();

	local direction = player- position;
	direction:normalize();
	local force = direction * 9;
	sphere:add_force(force.x, force.y, 0.0);

    if position.z < -200 then
        game:remove_self();
    end

	return true;
end


function handle_message(message, params, from)
	local string_message = message:get_string();
	
	if string_message == "levitate" then
		local force = params:get_vector(0);
		sphere:add_force(force.x, force.y, force.z);	
	end
	
end

function destroy()	
	game:play_sound("explode", 1.0, false);
	game:remove_object(sphere);	
	sphere = nil;
    print "Destroy Sphere";
end
