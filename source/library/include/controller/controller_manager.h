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

#ifndef NCCENTRIFUGE_CONTROLLER_MANAGER_H
#define NCCENTRIFUGE_CONTROLLER_MANAGER_H
#include <functional>

#ifdef WIN32
	#include <map>
#else
	#include <tr1/unordered_map>
#endif

#include <boost/utility.hpp>
#include <boost/bind.hpp>
#include <boost/functional/hash.hpp>
#include "controller/controller_interface.h"
#include "utilities/cache.h"
#include "utilities/multi_cache.h"
#include "controller/message_manager.h"
namespace ncc {
namespace controller
{    
    ///@defgroup controllers Controllers
    ///These are classes which have to do with controllers.

    ///@defgroup interfaces Interfaces
    ///These are classes which are interfaces.
    ///
    ///Interfaces are classes which are meant to be derived from. They are 
    ///normally abstract classes. Interfaces define a set of methods which are 
    ///common among those type of objects. By creating a class and deriving from
    ///an abstract_interface, it allows that class to be plugged in to a specific part 
    ///of the library.

    ///Manages controllers.
    ///
    ///Any subclass of ncc::controller::abstract_interface can be managed. The 
    ///controller manager allows for many useful things. Controllers can be 
    ///created manually or from a prototype. 
    ///\n\n
    ///It is recommended that each controller take a reference to the controller
    ///manager so that it can use the controller managers methods. Besides being 
    ///able to add and remove controllers, controllers can send messages to each 
    ///other using the controller managers various send_message* methods.
    ///@ingroup managers
    ///@ingroup controllers
    class manager : boost::noncopyable
    {
        public:
            ///Adds a prototype for creation purposes.
            ///
            ///There are several requirements for a prototype
            ///     - A prototype controller must not be initialized! Meaning it is 
            ///		  a controller which has not had its initialize method called.
            ///     - clone_prototype method for the controller must be coded to
            ///		  return a newly created controller of the prototype.        
            ///@param prototype The prototype that needs to be added. If the 
            ///prototype has already been added then nothing happens.
            ///@param name The name for the prototype so that you can refer to it 
            ///somehow. 
            void add_prototype(const std::string& name, ptr& prototype);

            ///Adds a controller to manage.
            ///
            ///Adds a controller to manager. It is recommended that the new 
            ///controllers initialize method is called. The controller will be added 
            ///to the root node of the controller tree. 
            ///@param new_controller It is recommended that controllers initialize 
            ///method be called before adding the new controller.
            void add_controller(ptr new_controller);

            ///Adds a controller and initializes it with specific parameters.
            ///
            ///The controller is added and it's initialize method is called with the 
            ///parameters specified. This method exists for convenience because a 
            ///controller is usually initialized after it is added.
            ///@param new_controller The new controller must not have its initialize
            ///method called before adding.
            ///@param params The parameters you want to initialize the controller 
            ///with. @see ncc::parameters.
            void add_controller(ptr new_controller, const parameter_list& params);

            ///Adds a controller as a child of another controller.
            ///
            ///The parent parameter must be a node that is already controlled by the
            ///controller manager. If the parent is not managed by the manager, the 
            ///new controller will just be a new child of the parent but will not be 
            ///managed by the manager. 
            ///@param new_controller It is recommended that the controllers 
            ///initialize method is called before adding.
            ///@param parent A controller which 
            void add_controller(ptr new_controller, ptr parent);

            ///Adds a controller as a child of another controller and initializes it 
            ///with the specified parameters.
            void add_controller(ptr new_controller, const parameter_list& params, ptr& parent);

            ///Creates a controller of the type specified with the parameters 
            ///specified and adds it to the manager.
            ///
            ///This method can add any controller which was added as a prototype by 
            ///the add_prototype method. If there are any errors in creating the 
            ///controller, then a null weak_ptr will be returned.
            ///@param type The type of controller which you want to create. The 
            ///controller type must be in the prototype list.
            ///@param params The parameters which the controller will be initialized 
            ///with.
            weak_ptr add_controller(const std::string& type, const parameter_list& params);

            ///Creates a controller of the type specified and adds it as a child of 
            ///another controller initialized with the specified parameters.
            weak_ptr add_controller(const std::string& type, const parameter_list& params, ptr parent);

            ///Creates a control of the type specified.
            ///
            ///This method creates a controller of the type specified initialized 
            ///with the parameters specified. The controller is not added to the 
            ///manager.
            ///@return boost::shared_ptr<ncc::controller::abstract_interface> of the newly 
            ///created controller. The pointer will be 0 if an error accured.
            ptr create_controller(const std::string& type, const parameter_list& params);


            ///Removes a controller with the specified name from the manager. 
            ///@param name The name of the controller you would like to find
            ///@return True if the controller was removed successfully.
            void remove_controller(const std::string& name);

            ///Removes a controller with the specified id.
            ///@param value 
            void remove_controller(id_type value);

            ///removes a specific controller.
            ///
            ///This method essentially calls the controller's remove_self method.
            static void remove_controller(ptr& controller);

            ///Finds a controller with the specified name.
            ///
            ///Returns a controller with the specified name. The first one found is 
            ///returned. 
            weak_ptr find_controller(const std::string& name);

            ///Finds a controller with the specified id value
            ///
            ///Returns a controller of the specified name. First one found is 
            ///returned.
            weak_ptr find_controller(const id_type& value);

            ///Finds controllers of the specified name.
            ///
            ///This method unlike the find_controller methods can find more than one 
            ///controller of the specified type.
            ///@param name The name of the controller you want to find.
            ///@param controllers The list you want filled with the found 
            ///controllers.
            void find_controllers(const std::string& name, list& controllers);

            ///Finds controllers of the specified type.
            ///
            ///This finds controllers of the specified type. This method can fill a 
            ///list with all controllers of the type.
            ///@param type The type of the controller you want to find.
            ///@param controllers The list you want filled with the found 
            ///controllers.
            void find_controllers_by_type(const std::string& type, list& controllers);    

            ///Calls all of the controller's control methods.
            ///
            ///This method goes through the controller tree and calls all control 
            ///methods. This method should be called inside the main game loop so 
            ///that all controllers managed by the controller manager have their 
            ///logic called.
            void step();

            ///Sends a message to other controllers with the specified name.
            ///
            ///This method is usually called within a controller. Therefore the 
            ///sender parameter is the 'this' pointer.
            ///\n\n
            ///Example 1: controller_manager.send_message(this, "player", message::parcel("Kill")); //called within another controller.
            ///\n\n
            ///Example 2: controller_manager.send_message(this, "player", message::parcel(43)); //where 43 means something to player.
            ///@param sender This is a pointer of the sender.
            ///@param name The name of the controller you want to send a message to.
            ///@param message This is a ncc::controller::message::parcel which 
            ///contains the message.
            ///@see ncc::controller::message::parcel
            void send_message(abstract_interface* sender,
                    const std::string& name,
                    const message::parcel& message);

            ///Sends a message to other controllers with the specified name.
            ///
            ///This method is usually called within a controller. Therefore the 
            ///sender parameter is the 'this' pointer.
            ///\n\n
            ///Example 1: manager.send_message(this, "player", "Kil"); //called within another controller.
            ///\n\n
            ///Example 2: manager.send_message(this, "player", 43); //where 43 means something to the player.
            ///\n\n
            ///The message parameter can be any type of object!
            ///\n\n
            ///Example 3: manager.send_message(this, "player", "push")(20)(3)(3); //push the player with 3 parameters.
            ///\n\n
            ///In example 3 demonstrates can specify as many parameters of any type 
            ///as you want by adding more () after the function call.
            ///@param sender This is a pointer of the sender.
            ///@param name The name of the controller you want to send a message to.
            ///@param message This is the message you would like to send to the 
            ///controllers.
            ///@see ncc::parameter
            message::parcel& send_message(abstract_interface* sender, 
                    const std::string& name, 
                    const parameter& message);

            ///Sends a message to other controllers with the specified name.
            ///
            ///This method is usually called within a controller. Therefore the 
            ///sender parameter is the 'this' pointer.
            ///\n\n
            ///Example: manager.send_message(this, "player", "push", parameters(3, 4, 5)); //called within another controller.
            ///\n\n
            ///The message and params parameter can be any type of object! to create the parameter_list, use ncc::parameters
            ///function.
            ///@param sender This is a pointer of the sender.
            ///@param name The name of the controller you want to send a message to.
            ///@param message This is the message you would like to send to the 
            ///controllers.
            ///@param params Use ncc::parameters function to create the parameters
            ///@see ncc::parameter
            ///@see ncc::parameter_list
            void send_message(abstract_interface* sender, 
                    const std::string& name, 
                    const parameter& message, 
                    const parameter_list& params);

            ///Sends a message to a specific controller.
            void send_message(abstract_interface* sender,
                    ptr& recipient,
                    const message::parcel& message);
            ///Sends a message to a specific controller.
            message::parcel& send_message(abstract_interface* sender, 
                    ptr& recipient, 
                    const parameter& message);        
            ///Sends a message to a specific controller.
            void send_message(abstract_interface* sender, 
                    ptr& recipient, 
                    const parameter& message, 
                    const parameter_list& params);

            ///Sends a message to all controllers of a certain type.
            void send_message_to_all(abstract_interface* sender,
                    const std::string& type, 
                    const message::parcel& message);
            ///Sends a message to all controllers of a certain type.
            message::parcel& send_message_to_all(abstract_interface* sender, 
                    const std::string& type, 
                    const parameter& message);        
            ///Sends  a message to all controllers of a certain type.
            void send_message_to_all(abstract_interface* sender, 
                    const std::string& type, 
                    const parameter& message, 
                    const parameter_list& params);  

            ///Sends a message to all controllers.
            void send_message_to_all(abstract_interface* sender, 
                    const message::parcel& message);
            ///Sends a message to all controllers.
            message::parcel& send_message_to_all(abstract_interface* sender,
                    const parameter& message);
            ///Sends a message to all controllers.
            void send_message_to_all(abstract_interface* sender,
                    const parameter& message,
                    const parameter_list& params);
            ///Default constructor.
            manager();

        private:

            ///Finds a prototype of a specific type and returns a weak_ptr to it.
            weak_ptr find_prototype(const std::string& type);    

            ///Creates a flat list of controllers in depth first order.
            void create_flat_list(list& controllers);
        private:

#ifdef WIN32
            typedef std::map<std::string, ptr> prototype_map;
#else
            ///The prototype_map is a hash map for fast lookup.
            typedef std::tr1::unordered_map<std::string, ptr, boost::hash<std::string> > prototype_map;
#endif

            ptr root_controller; 
            prototype_map prototypes;
            message::manager message_manager;
    };

    namespace hidden
    {
        ///This is the root controller used by ncc::controller::manager
        ///
        ///This controller is the root controller which all other controllers are added 
        ///as children to in ncc::controller::manager. The type is "root" and 
        ///type_id is 0. The name is also root. Therefore user controllers 
        ///should not have a type = "root" and id = 0.
        class simple_root_controller : public abstract_interface 
        {
            public:
                std::string get_type() {return  "root";}
                int get_type_id() {return 0;}
                simple_root_controller() : abstract_interface() {}
                bool initialize(const parameter_list&){return false;}
        };
    }
}//namespace controller
}//namespace ncc
#endif

