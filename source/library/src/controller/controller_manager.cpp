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

#include "controller/controller_manager.h"
namespace ncc {
namespace controller
{
    manager::manager()
    {
        root_controller = ptr(new hidden::simple_root_controller());
    }
    void manager::add_controller(ptr new_controller)
    {
        add_controller(new_controller,root_controller);
    }
    void manager::add_controller(ptr new_controller, const parameter_list& params)
    {
        add_controller(new_controller, params, root_controller);
    }
    void manager::add_controller(ptr new_controller, ptr parent)
    {
        if(!parent || !new_controller) return;
        parent->add_child(new_controller);
    }
    void manager::add_controller(ptr new_controller, const parameter_list& params, ptr& parent)
    {
        if(!parent || !new_controller) return;
        
        if(new_controller->initialize(params) == false) return;
        
        parent->add_child(new_controller);
    }
	void manager::add_prototype(const std::string& name, ptr& prototype)
    {
        if(prototype && !name.empty()) prototypes[name] = prototype;
    }
	
    weak_ptr manager::find_prototype(const std::string& type)
    {
        prototype_map::iterator found_prototype = prototypes.find(type);
        return found_prototype != prototypes.end() ? found_prototype->second : ptr();
    }

    ptr manager::create_controller(const std::string& type, const parameter_list& params)
    {
        
        ptr controller_to_create(find_prototype(type));

        //because we could not find a controller of the specified type to clone, we return 0
        if(!controller_to_create) return ptr();
        
        ptr new_controller(controller_to_create->clone_prototype());
        //because the controller was unable to be cloned, we return 0
        if(!new_controller) return ptr();
        
        //because a controller can be configured using its initialize function, we do so. 
        //if it returns false then we had an error during initialization
        return new_controller->initialize(params) ? new_controller : ptr();
    }
    
    weak_ptr manager::add_controller(const std::string& type, const parameter_list& params)
    {
        return add_controller(type, params, root_controller);
    }

    weak_ptr manager::add_controller(const std::string& type, const parameter_list& params, ptr parent)
    {
        ptr new_controller = create_controller(type, params);
        add_controller(new_controller, parent);
    
        return weak_ptr(new_controller);
    }

    bool remove_controller_by_name(ptr& controller, const std::string& name)
    {
        if(controller->get_name().size() && controller->get_name() == name)
        {
            manager::remove_controller(controller);
            return false;
        }
        return true;
    }
    
    void manager::remove_controller(const std::string& name)
    {
        using namespace boost;
        tree::transverse_depth_first(root_controller, boost::bind<bool>(remove_controller_by_name, _1, name));
    }
    

    bool remove_controller_by_id(ptr& controller, id_type id) 
    {
        if(controller->get_id() == id)
        { 
            manager::remove_controller(controller);
            return false;
        }
        return true;
    }
    
    void manager::remove_controller(id_type id)
    {
        tree::transverse_depth_first(root_controller, boost::bind<bool>(remove_controller_by_id, _1, id));
    }
    
    void manager::remove_controller(ptr& controller)
    {
        if(controller) controller->remove_self();
    }
    
    inline bool controller_has_name(ptr& cntr, const std::string& name) 
    {
        return cntr->get_name().size() && cntr->get_name() == name;
    }
    
    inline bool controller_has_id(ptr& controller, id_type id)  {return controller->get_id() == id;}    
    inline bool controller_has_type(ptr& cntr, const std::string& type) { return cntr->get_type() == type;}
    
    weak_ptr manager::find_controller(const std::string& name)
    {
        using namespace boost;
        return tree::depth_first_search(root_controller, bind(controller_has_name, _1, ref(name)));
    }
    weak_ptr manager::find_controller(const id_type& controller_id)
    {
        using namespace boost;
        return tree::depth_first_search(root_controller, bind(controller_has_id, _1, ref(controller_id)));
    }
    
    bool add_if_name_is(ptr& controller, list& controllers, const std::string& name)
    {
        if(controller->get_name() == name) controllers.push_back(controller);
        return true;
    }
    void manager::find_controllers(const std::string& name, list& controllers)
    {
        using namespace boost;
        tree::transverse_depth_first(root_controller, bind<bool>(add_if_name_is, _1, ref(controllers), ref(name)));
    }
    
    bool add_if_type_is(ptr& controller, list& controllers, const std::string& type)
    {
        if(controller->get_type() == type) controllers.push_back(controller);
        return true;
    }
    void manager::find_controllers_by_type(const std::string& type, list& controllers)
    {
        using namespace boost;
        tree::transverse_depth_first(root_controller, bind<bool>(add_if_type_is, _1, ref(controllers), ref(type))); 
    }
    
    bool add_to_list(ptr& controller, list& controllers)
    {
        if(controller) controllers.push_back(controller);
        return true;
    }
    void manager::create_flat_list(list& controllers)
    {
        using namespace boost;
        tree::transverse_depth_first(root_controller, bind<bool>(add_to_list, _1, ref(controllers)));
    }
    
    bool control(ptr& controller)
    {
        return controller->control();
    }
    
    bool should_remove(ptr& controller)
    {
        return controller ? !controller->is_alive() : true;
    }
    
    void manager::step()
    {
        //walk through tree and control each object
        tree::transverse_depth_first(root_controller, control);
        //send messages to appropriate objects
        message_manager.send_messages(root_controller);
        //remove any objects which are not alive anymore
        tree::remove_if(root_controller, should_remove); 
    }
    
    void manager::send_message(abstract_interface* sender,
                                const std::string& name,
                                const message::parcel& message)
    {
        if(!sender) return;

        using namespace boost;        
        message::ptr new_message(
                new message::message(
                    sender, 
                    bind<bool>(controller_has_name, _1, name),
                    message));
    
        message_manager.add_message(new_message);
    }
    
    message::parcel& manager::send_message(abstract_interface* sender, 
                                const std::string& name, 
                                const parameter& message)
    {

        using namespace boost;        
        message::ptr new_message(
                new message::message(
                    sender, 
                    bind<bool>(controller_has_name, _1, name),
                    message::parcel(message)));
    
        message_manager.add_message(new_message);
        return new_message->message_parcel;
    }
    
    void manager::send_message(abstract_interface* sender, 
                                const std::string& name, 
                                const parameter& message, 
                                const parameter_list& params)
    {
        if(!sender) return;

        using namespace boost;        
        message::ptr new_message(
                new message::message(
                    sender, 
                    bind<bool>(controller_has_name, _1, name),
                    message::parcel(message, params)));
    
        message_manager.add_message(new_message);
    }
    
    
    inline bool is_controller(ptr& controller_1, ptr& controller_2)
    {
        return controller_1 == controller_2;
    }
    
    void manager::send_message(abstract_interface* sender,
                                ptr& to,
                                const message::parcel& message)
    {
        if(!sender) return;

        using namespace boost;        
        message::ptr new_message(
                new message::message(
                    sender, 
                    bind<bool>(is_controller,  _1 , to),
                    message));
    
        message_manager.add_message(new_message);
    }
    
    message::parcel& manager::send_message(abstract_interface* sender, 
                                ptr& to, 
                                const parameter& message)
    {

        using namespace boost;        
        message::ptr new_message(
                new message::message(
                    sender, 
                    bind<bool>(is_controller,  _1 , to),
                    message::parcel(message)));
    
        message_manager.add_message(new_message);
        return new_message->message_parcel;
    }
        
    void manager::send_message(abstract_interface* sender, 
                                ptr& to, 
                                const parameter& message, 
                                const parameter_list& params)
    {
        if(!sender) return;

        using namespace boost;        
        message::ptr new_message(
                new message::message(
                    sender, 
                    bind<bool>(is_controller,  _1 , to),
                    message::parcel(message, params)));
    
        message_manager.add_message(new_message);
    }

    
    void manager::send_message_to_all(abstract_interface* sender,
                                        const std::string& type, 
                                        const message::parcel& message)
    {
        if(!sender) return;

        using namespace boost;        
        message::ptr new_message(
                new message::message(
                    sender, 
                    bind<bool>(controller_has_type, _1, type),
                    message));
    
        message_manager.add_message(new_message);
    }


    message::parcel& manager::send_message_to_all(abstract_interface* sender, 
                                        const std::string& type, 
                                        const parameter& message)
    {                
        using namespace boost;        
        message::ptr new_message(
                new message::message(
                    sender, 
                    bind<bool>(controller_has_type, _1, type),
                    message::parcel(message)));
    
        message_manager.add_message(new_message);    
        return new_message->message_parcel;
    }
        
    void manager::send_message_to_all(abstract_interface* sender, 
                                        const std::string& type, 
                                        const parameter& message, 
                                        const parameter_list& params)
    {
        if(!sender) return;

        using namespace boost;        
        message::ptr new_message(
                new message::message(
                    sender, 
                    bind<bool>(controller_has_type, _1, type),
                    message::parcel(message, params)));
    
        message_manager.add_message(new_message);
    }
  
    bool returns_true(ptr controller) {return true;}
  
    void manager::send_message_to_all(abstract_interface* sender, 
                                        const message::parcel& message)
    {
        if(!sender) return;

        using namespace boost;        
        message::ptr new_message(
                new message::message(sender, 
                    returns_true,
                    message));
    
        message_manager.add_message(new_message);
    }

    message::parcel& manager::send_message_to_all(abstract_interface* sender,
                                        const parameter& message)
    {                
        using namespace boost;        
        message::ptr new_message(
                new message::message(
                    sender, 
                    returns_true,
                    message::parcel(message)));
    
        message_manager.add_message(new_message);
        return new_message->message_parcel;
    }

    void manager::send_message_to_all(abstract_interface* sender,
                                        const parameter& message,
                                        const parameter_list& params)
    {
        if(!sender) return;

        using namespace boost;        
        message::ptr new_message(
                new message::message(
                    sender, 
                    returns_true,
                    message::parcel(message,params)));
    
        message_manager.add_message(new_message);    
    }
}//namespace controller
}//namespace ncc
