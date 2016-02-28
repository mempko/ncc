--Pressing 'f' will send a force to all object...sor of levitating them.
--Pressing 'd' will destroy all objects.
--Pressing 'b' will drop a box in the middle of the world


local world = nil;

--This variable will determine if a key is being held down
local keyPressed = false;


local gravity = 0;
function initialize(params)
	game:register_sound("Data/explode.wav", "explode");
	game:register_sound("Data/fire.wav", "shoot");

	game:set_int_property("gravity", gravity);
	game:set_vector_property("player", ncc.vector(0,0,0));

	--local fileName = "Data/level.obj";
	local fileName = "Data/level1.obj";
	local mass =0;
	local pos = ncc.vector(0, 0, 0);
	local world = game:create_mesh(fileName, pos, mass);			
		
	world:load_texture("Data/level1.jpg");
	world.name = "ground";
    world.friction = 2.0;
	game:register_script("box", "Data/box.lua");
	game:register_script("camera", "Data/camera.lua");
	game:register_script("sphere", "Data/sphere.lua");

	local position = ncc.parameter(ncc.vector(0, 19, 0));
	game:add_controller("camera", ncc.parameters(position));
	
	CreateSpheres();
	return true;
end


function control()
	if game:key_pressed("f") then			
		local force = ncc.parameter(ncc.vector(0,0,30));
		local command = ncc.parameter("levitate");
		game:send_message_to_all(command,ncc.parameters(force));
	
	end
	if game:key_pressed("d") then
		game:send_message_to_all(ncc.parameter("destroy"));
	end
	
	if game:key_pressed("g") then
		if keyPressed == false then
			if gravity == 1 then
				gravity = 0;
			else
				gravity = 1;
			end
			game:set_int_property("gravity", gravity);
			local val = game:get_int_property("gravity");	
			print("Gravity ", val);
		end
		keyPressed = true;
	elseif game:key_pressed(" ") then
		if keyPressed == false then
			ShootBox();
		end
		keyPressed = true;
	elseif game:key_pressed("s") then
		if keyPressed == false then
			CreateSpheres();
		end
		keyPressed = true;
	else
		keyPressed = false;
	end
	return true;

end

function handle_message(message, params, from)

end


function destroy()	
    game:remove_object(world);
	world = nil;
    print "Destroy World";
end


function CreateSpheres()

	local enemies = 20;
	while enemies > 0 do
		local x = ncc.rand() * 10 + 19;
		local y = ncc.rand() * 10;
		local z = 2;
		local position = ncc.parameter(ncc.vector(x, y, z));
		local radius = ncc.parameter(1);
		local mass = ncc.parameter(1);
		game:add_controller("sphere", ncc.parameters(position, radius, mass));
		enemies = enemies - 1;
	end
end

function ShootBox()
	
	local camera_pos = ncc.parameter(game:get_camera_position());
	local camera_orientation = game:get_camera_orientation();
	local size = ncc.parameter(ncc.vector(1, 1, 1));	
	local mass = ncc.parameter(1.0);
	local camera_dir = camera_orientation * ncc.vector(0.0, 0.05, -1.0);
	local vel = camera_dir * 30;
	local velocity = ncc.parameter(vel);				
			
	game:add_controller("box", ncc.parameters(camera_pos, size, mass, velocity));
	game:play_sound("shoot", 1.0, false);
end
