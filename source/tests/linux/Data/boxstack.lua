--Pressing 'a' will add a box
local ground = nil;

--This variable will determine if a key is being held down
local keyPressed = false;

local gravity = 0;
function initialize(params)
	--ground = game:create_box(ncc.vector(0,0,-8), ncc.vector(100, 100, 5), 0);
	--ground:load_texture("Data/Brown.png");
	local mass =0;
	local pos = ncc.vector(0, 0, 0);
	ground = game:create_mesh("Data/world.obj", pos, mass);			
		
	ground:load_texture("Data/max.jpg");

    ground.name = "ground";
    ground.bounce = 0.5;	
    ground.friction = 1.5;	
	game:register_script("box", "Data/box.lua");
	
	return true;
end

function control()
	if game:key_pressed("f") then			
		local force = ncc.parameter(ncc.vector(0,0,30));
		local command = ncc.parameter("levitate");
		game:send_message_to_all(command,ncc.parameters(force));
	end
    
    if game:key_pressed("a") then
        if keyPressed == false then
            AddBox();
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
	game:remove_object(ground);
	ground = nil;	
end

function AddBox()
	
	local pos = ncc.parameter(ncc.vector(0,0,100));
	local size = ncc.parameter(ncc.vector(5, 5, 5));	
	local mass = ncc.parameter(1.0);
	local velocity = ncc.parameter(ncc.vector(0.0,0.0,0.0));				
			
	game:add_controller("box", ncc.parameters(pos, size, mass, velocity));
end

