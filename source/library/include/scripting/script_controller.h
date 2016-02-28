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

#ifndef NCCENTRIFUGE_SCRIPT_CONTROLLER_H
#define NCCENTRIFUGE_SCRIPT_CONTROLLER_H
#define BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
#include "scripting/script.h"
#include "controller/controller_interface.h"
#include "object/object_manager.h"
#include "object/osg_ode/osg_ode.h"
#include "utilities/property_manager.h"
#include "sound/oal_manager.h"
#include <list>
#include <iostream>
#include <functional>
namespace ncc {
namespace lua
{
    ///This class is a controller which uses the lua scripting language 
    ///for its logic.
    ///
    ///This controller allows a lua script to control the logic of a game. 
    ///Much of the NCCentrifuge library is wrapped to be used in Lua. See 
    ///http://www.lua.org for more information on Lua. So the many things you
    ///can do in a C++ controller you can also do in a Lua controller.
    class controller : public ncc::controller::abstract_interface
    {
        public:
            typedef boost::shared_ptr<controller> ptr;
            typedef boost::weak_ptr<controller> weak_ptr;

            ///The only constructor.
            ///
            ///This constructor takes many parameters. It loads a specified script
            ///from a file and binds the NCCentrifuge library to it.
            ///@param file The file name of the Lua script.
            ///@param bind_func This should normally be 
            ///ncc::lua::utilities::bind_ncc.
            controller(const std::string& file,
                    boost::function<void(lua_State*)> bind_func,
                    osg::manager& osg_manager, 
                    ode::manager& ode_manager,
                    oal::manager& oal_manager,
                    object::manager& object_manager,
                    ncc::controller::manager& controller_manager,
                    property::manager& property_manager);

            ///The standard controller initialize function. This method loads the
            ///script into memory and executes any code not in a function. It then
            ///passes all the parameters to an initialize function in the lua 
            ///script. The initialize function in the script should have one 
            ///parameter called "params". The whole script has access to "game" 
            ///object which has many useful methods.
            bool initialize(const parameter_list& param);

            ///Calls the control function within the lua script. The control 
            ///function has no parameters
            bool control();

            ///Calls the handle_message function within the script.
            ///
            ///This calls the handle_message function and passes it a message, a
            ///parameter list, and the object which sent the message.
            void handle_message(const parameter& message, const parameter_list& params, controller::abstract_interface& from);

            ///Returns the type of the controller.
            std::string get_type() {return "script_controller";}

            osg::manager& osg_manager() { return osg_mgr;}
            ode::manager& ode_manager() { return ode_mgr;}
            oal::manager& oal_manager() { return oal_mgr;}
            object::manager& object_manager() { return object_mgr;}
            ncc::controller::manager& controller_manager() { return controller_mgr;}
            property::manager& property_manager() {return property_mgr;}

            ///This method calls remove_self. 
            ///
            ///This method is here so that a user can call "game:remove_self" 
            ///within the script to destroy the controller.
            void remove();

            ///Sets the collision callback for the ode::object.
            ///
            ///In a script call "game:set_collision_callback(object, "some_function")"
            ///Then that function will be called when the object collides. That 
            ///function must take one object parameter. 
            void set_collision_callback(ode::object* object, const std::string& function);

            ///This is the function which is called when an ode::object within
            ///the script collides.
            ///
            ///If an object in the script has its collision callback set, then when
            ///it collides, this method is called. This method then sorts out which
            ///function within the script to call.
            bool collision_callback(ode::collision_info info);

            ///Returns the function pointer to the bind function.
            const boost::function<void(lua_State*)>& get_bind_function() const {return bind_function;}
            ~controller();
        protected:
            controller::abstract_interface* clone_prototype();

        private:

            typedef std::map<ncc::object::abstract_interface*, std::string> callback_map;
            callback_map callbacks;
            osg::manager& osg_mgr;
            ode::manager& ode_mgr;
            oal::manager& oal_mgr;
            object::manager& object_mgr;
            ncc::controller::manager& controller_mgr;
            property::manager& property_mgr;
            script lua_script; 
            std::string file_name;
            boost::function<void(lua_State*)> bind_function;

    };

}//namespace lua
}//namespace ncc
#endif

