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


#ifndef NCCENTRIFUGE_WORLD_OBJECT_INTERFACE_H
#define NCCENTRIFUGE_WORLD_OBJECT_INTERFACE_H

#include <exception>
#include <string>
#include <boost/any.hpp>
#include <boost/weak_ptr.hpp>
#include "elements/tree.h"
#include "elements/parameter.h"
#include "elements/id.h"

namespace ncc {
namespace controller
{
    //forward declaration
    class manager;

    ///Controller abstract_interface
    ///
    ///A controller::abstract_interface is an abstract class you derive from to 
    ///create a controller compatible with the controller::manager. 
    ///A controller represents some logic in the game. 
    ///
    ///
    ///Controller abstract_interface derives from elements::is_node<> which is 
    ///makes a controller a node in a tree. @see nccentrifuge::is_node
    ///Controller abstract_interface also derives from elements::id_type which 
    ///allows each controller created to have a different id_type number.
    ///This is useful for searching and retrieving a controller from the 
    ///controller::manager by id.
    ///@see nccentrifuge::id_type
    ///@ingroup controllers
    ///@ingroup interfaces
    class abstract_interface : public tree::is_node<abstract_interface>, public id_type
    {
        public:
            ///Returns the type of the controller as a wide string.
            virtual std::string get_type() {return std::string();}
            ///Returns the type of the controller as a number. 
            virtual int get_type_id() {return -1;}

            ///Handles messages sent by other controllers.
            ///
            ///Override to handle messages sent by other controllers. To send a
            /// message, you call the controller::managers send_message member. 
            ///If you want your controller to handle any messages then 
            ///overriding this method is important.
            ///@param message The actual message sent @see ncc::parameter
            ///@param params The parameters of the message if any. 
            ///@see ncc::parameter_list
            ///@param from The sender of the message.
            virtual void handle_message(const parameter& message, const parameter_list& params, abstract_interface& from) {}

            ///Initializes a controller based on parameters sent.
            ///
            ///Override to handle custom initialization of your controller. If 
            ///you want your controller to abstract_interface with other controllers 
            ///nicely then consider overriding this method and treat it as your 
            ///constructor. Overriding this, in combination with clone_prototype 
            ///allows lua scripts to create your object.
            ///@return True if success and false if failure.
            virtual bool initialize(const parameter_list&) = 0;

            ///Returns the name of the object.
            virtual const std::string& get_name(){return name_holder;}

            ///Sets the name of the object
            virtual void set_name(const std::string& value) {name_holder = value;}

            ///This is where all the logic of your controller should be.
            ///
            ///Override to handle logic. This is called a specific amount of 
            ///times a second. Game logic such as enemy AI and player control 
            ///can go here.
            ///@return True if control should continue to the children, 
            ///otherwise false.
            virtual bool control() {return true;}

            ///Returns true if the controller is still alive.
            ///
            ///This method is primarily used by the controller manager to 
            ///determine if a controller should be destroyed. A controller can 
            ///destroy itself using the remove_self method. It is advisable not 
            ///to use the controller's remove_child method to remove a 
            ///controller because it might cause the application to crash 
            ///because it invalidates iterators.
            bool is_alive() const {return alive;}

            ///This is the default constructor for the abstract_interface.
            ///
            ///It is important to call this constructor in derived classes 
            ///constructor. When you derive from ncc::controller::abstract_interface
            ///and have custom constructors, call this constructor at the end of 
            ///the initialization list.
            explicit abstract_interface() : name_holder(), alive(true), tree::is_node<abstract_interface>(this), id_type(){};

            virtual ~abstract_interface(){};			
        protected:
            //Give the manager class access to the clone_prototype method.
            friend class manager;

            ///Returns a pointer to a newly created clone on the heap
            ///
            ///This function should return a clone of the controller. The clone 
            ///should be in an uninitialized state. In other words, the initialize
            ///method must not be called! However, the clone should be in a 
            ///state such that when the initialize method is called, it can 
            ///succeed. 
            ///@return A clone of the controller in an uninitialized state. 
            virtual abstract_interface* clone_prototype() {return 0;}
            void remove_self(){alive = false;}
        private:
            std::string name_holder; ///< Holds the name of the controller. 
            bool alive; ///< If false then the controller is destroyed at the 
            ///end of ncc::controller::manager::step() method.

    };

    ///A boost::shared_ptr of a abstract_interface object.
    ///
    ///This typedef is used by many components of the library. 
    ///Instead of writing boost::shared_ptr<ncc::controller::abstract_interface>, 
    ///please use ncc::controller::ptr instead;
    typedef boost::shared_ptr<abstract_interface> ptr; 

    ///A weak pointer to an abstract_interface object.
    ///
    ///Instead of writing boost::weak_ptr<ncc::controller::abstract_interface> use
    ///weak_ptr instead. This is used extensively by the library. A weak 
    ///pointer can only be created from a shared_ptr. In other words, a 
    ///weak_ptr can only be created from a ptr object. When you would like to 
    ///access the object that weak_ptr points to, please use the following
    ///method (note that ncc::controller namespace is not used here to make 
    ///things shorter).
    ///     - if(ptr controller = my_weak_ptr.lock()) { /*... do stuff...*/}
    ///
    ///Please see http://www.boost.org for more documentation on 
    ///boost::shared_ptr and boost::weak_ptr.
    typedef boost::weak_ptr<abstract_interface> weak_ptr;

    ///A list is an std::list of controller_ptr's...as you can see.
    typedef std::list<ptr> list;
}//controller
}//nccentrifuge
#endif

