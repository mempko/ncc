/*
 * Copyright (C) 2016  Maxim Noah Khailo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give 
 * permission to link the code of portions of this program with the 
 * Botan library under certain conditions as described in each 
 * individual source file, and distribute linked combinations 
 * including the two.
 *
 * You must obey the GNU General Public License in all respects for 
 * all of the code used other than Botan. If you modify file(s) with 
 * this exception, you may extend this exception to your version of the 
 * file(s), but you are not obligated to do so. If you do not wish to do 
 * so, delete this exception statement from your version. If you delete 
 * this exception statement from all source files in the program, then 
 * also delete it here.
 */

#include "scripting/script_controller.h"
#include <sstream>
using namespace luabind;
namespace ncc {
namespace lua
{
    int print_file_and_line(lua_State* state)
    {
		
	   std::ostringstream msg;
       lua_Debug d;
	   
       lua_getstack(state, 1, &d);
       lua_getinfo(state, "Sln", &d);
	   
       std::string err = lua_tostring(state, -1);
       
	   lua_pop(state, 1);
	   //int currentline = d.currentline;
       msg << d.short_src;// << ":" << currentline;

	   if (d.name != 0 && d.namewhat != 0)
       {
          msg << "(" << d.namewhat << " " << d.name << ")";
       }
	 
       msg << " " << err << '\n';
	   
       lua_pushstring(state, msg.str().c_str());
	   std::cout << msg.str();
	   return 1;
    }


    controller::controller(const std::string& file,
                                boost::function<void(lua_State*)> bind_func,
                                osg::manager& osg_manager, 
                                ode::manager& ode_manager,
								oal::manager& oal_manager,
                                object::manager& object_manager,
                                ncc::controller::manager& controller_manager,
								property::manager& property_manager)
                                : osg_mgr(osg_manager),
                                ode_mgr(ode_manager),
								oal_mgr(oal_manager),
                                object_mgr(object_manager),
                                controller_mgr(controller_manager),
								property_mgr(property_manager),
                                file_name(file),
                                bind_function(bind_func)
    {
        bind_func(lua_script.state());		
    }
    bool controller::initialize(const parameter_list& param)
    {
		if(!is_alive()) return false;
		set_pcall_callback(print_file_and_line);
		luabind::globals(lua_script.state())["game"] = this;
        if(lua_script.load(file_name) == false) return false;
        try
        {
			return call_function<bool>(lua_script.state(),"initialize", param);
        }
        catch(luabind::error& e)
        {
            std::cout << "Lua Error: " << e.what() << std::endl;
            return false;
        }
    }
    bool controller::control()
    {
		if(!is_alive()) return false;
        try
        {
            return call_function<bool>(lua_script.state(), "control");
        }
        catch(luabind::error& e)
        {
            std::cout << "Lua Error: " << e.what() << std::endl;
            return false;
        }
    }
	
    void controller::handle_message(const parameter& message, const parameter_list& params, controller::abstract_interface& from)
    {
		if(!is_alive()) return;
        try
        {
			call_function<void>(lua_script.state(), "handle_message", message, params, &from);
        }
        catch(luabind::error& e)
        {
            std::cout << "Lua Error: " << e.what() << std::endl;
        }        
    }
	
    controller::abstract_interface* controller::clone_prototype() 
    { 
        controller::abstract_interface* cntr = 0;
        try
        {
            cntr = new controller(file_name, bind_function, osg_mgr, ode_mgr, oal_mgr, object_mgr, controller_mgr, property_mgr);
            return  cntr;
        }
        catch(luabind::error& e)
        {
            std::cout << "Lua Error: " << e.what() << std::endl;
            if(cntr) delete cntr;
            return 0;
        }
    }
    controller::~controller() 
    {	
		try
        {
            call_function<void>(lua_script.state(), "destroy");
        }
        catch(luabind::error& e)
        {
            std::cout << "Lua Error: " << e.what() << std::endl;
        }
		
    }
	void controller::remove()
	{	 
		remove_self();
	}
	
	bool controller::collision_callback(ode::collision_info info)
	{
		//if(!info.object_ptr) return true;
		callback_map::iterator found_callback = callbacks.find(info.object_1);
		
		//If there is no callback we assume the object will want to collide
		if(found_callback == callbacks.end()) return true; 
		try
		{
			//call the apropriate lua function for that object and send it a pointer to the 
			//object for which it collided with
			object::abstract_interface* object = info.object_2;
			
			return object ? call_function<bool>(lua_script.state(), found_callback->second.c_str(), object) : true;
		}
		catch(luabind::error& e)
        {
            std::cout << "Lua Error: " << e.what() << std::endl;
        }
		return true;
	}
	
	void controller::set_collision_callback(ode::object* object, const std::string& function)
	{
		if(!object) return;
		callbacks[dynamic_cast<object::abstract_interface*>(object)] = function;
		
		object->set_collision_callback(std::bind1st(std::mem_fun(&controller::collision_callback), this));
	}



}//namespace lua
}//namespace ncc
