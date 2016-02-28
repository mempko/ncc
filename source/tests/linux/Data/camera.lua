

local position = ncc.vector(0, 0, 30);
local direction = ncc.vector(0, -1, 0);
local orientation = ncc.quaternion();
orientation:create_from_euler(0 , 0 ,0);

local pre_x;
local pre_y;
local capsule = nil;

function initialize(params)
	pre_x = game:get_mouse_x();
	pre_y = game:get_mouse_y();
	local position = params:get_vector(0);
	capsule = game:create_invisible_capsule(position, 2, 2, 1);
	capsule.friction = 5.0;
	return true;
end


function control()
	
    local gravity = game:get_int_property("gravity");	
	if gravity == 1 then
		capsule:add_force(0, 0, 9.8); --eliminate gravity
	end
	
	local m_x = game:get_mouse_x();
	local m_y = game:get_mouse_y();

	local yaw = 0; 
	local pitch = 0; 
	position = capsule:get_position();

	if game:button_pressed(0) then
		yaw = (m_x - pre_x)*2;
		pitch = (m_y - pre_y) * 2;
	end
	pre_x = m_x;
	pre_y = m_y;
	
	if game:key_pressed('j') then
		yaw = 0.01;
	end
	if game:key_pressed('l') then
		yaw = -0.01;
	end
	if game:key_pressed('u') then
		pitch = -0.01;
	end
	if game:key_pressed('o') then
		pitch = 0.01;
	end
	

	local up_down = ncc.quaternion();
	local left_right = ncc.quaternion();
	up_down:create_from_euler(0 , pitch ,0);
	left_right:create_from_euler(0, 0, yaw);
	orientation = left_right * up_down;

	direction = orientation * direction;
	direction:normalize();	

	local forward_force = ncc.vector(0,0,0);
	forward_force = direction * 35;
	local back_force = ncc.vector(0,0,0);
	back_force = direction * -35;

	if game:key_pressed('i') then
		capsule:add_force(forward_force.x, forward_force.y, forward_force.z);
	end
	if game:key_pressed('k') then
		capsule:add_force(back_force.x, back_force.y, back_force.z);
	end
	if game:key_pressed('y') then
		capsule:add_force(0,0, 20);
	end

	looking_at = position+(direction);
	game:look_at(position, looking_at);
	game:set_vector_property("player",position);


	return true;
end

function handle_message(message, params, from)
	
end

function destroy()	
	game:remove_object(capsule);
	capsule = nil;
    print "Destroy Camera";
end
