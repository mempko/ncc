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

#include "scripting/script_utilities.h"
#include <luabind/operator.hpp>
#include <luabind/adopt_policy.hpp>
#include <luabind/dependency_policy.hpp>
#include <luabind/out_value_policy.hpp>

namespace ncc {
namespace lua {
namespace utilities
{
    using namespace luabind;
    scope bind_std()
    {
        return class_<std::string>("string");
    }
    
    scope bind_vector_3dd()
    {
        return class_<vector_3dd>("vector")
                .def(constructor<>())
                .def(constructor<const vector_3dd&>())
                .def(constructor<double, double, double>())
                .def("length", &vector_3dd::length)
                .def("length_squared",&vector_3dd::length_squared)
                .def("set", &vector_3dd::set)
                .def(const_self + other<vector_3dd>())
                .def(const_self - other<vector_3dd>())
                .def(const_self + double())
                .def(const_self - double())
                .def(const_self * other<vector_3dd>())
                .def("dot", &vector_3dd::dot)
                .def("cross", &vector_3dd::cross)
                .def(const_self * double())
                .def(const_self / double())
                .property("x", &vector_3dd::get_x, &vector_3dd::set_x)
                .property("y", &vector_3dd::get_y, &vector_3dd::set_y)
                .property("z", &vector_3dd::get_z, &vector_3dd::set_z)
                .def("normalized", &vector_3dd::normalized)
                .def("normalize", &vector_3dd::normalize);
    }
	scope bind_quaterniond()
	{
		return class_<quaterniond>("quaternion")
				.def(constructor<>())
				.def(constructor<const quaterniond&>())
				.def(constructor<double, double, double, double>())
				.def("set", &quaterniond::set)
				.def("create_from_euler", &quaterniond::create_from_euler)
				.property("x", &quaterniond::get_x, &quaterniond::set_x)
				.property("y", &quaterniond::get_y, &quaterniond::set_y)
				.property("z", &quaterniond::get_z, &quaterniond::set_z)
				.property("w", &quaterniond::get_w, &quaterniond::set_w)
				.def("normalize", &quaterniond::normalize)
				.def("normalized", &quaterniond::normalized)
				.def("get_axis_angle", &quaterniond::get_axis_angle)
				.def("create_from_axis_angle", &quaterniond::create_from_axis_angle)
				.def("get_inverse", &quaterniond::get_inverse)
				.def(const_self * other<quaterniond>())
				.def(const_self * other<vector_3dd>())
				.def(const_self + other<quaterniond>())
				.def(const_self - other<quaterniond>())
				.def(const_self / other<quaterniond>())
				.def("dot", &quaterniond::dot)
				.def("length", &quaterniond::length)
				.def("length_squared", &quaterniond::length_squared)
				.def("negated", &quaterniond::negated)
				.def("negate", &quaterniond::negate)
				.def("exponent", &quaterniond::exponent);
	}
         	

	vector_3dd get_object_position(ncc::object::abstract_interface* obj)
	{
		if(!obj)return vector_3dd();
		vector_3dd position;
		obj->get_position(position.x(), position.y(), position.z());
		return position;			
	}
	quaterniond get_object_orientation(ncc::object::abstract_interface* obj)
	{
		if(!obj) return quaterniond();
		quaterniond orientation;
		obj->get_orientation(orientation.x(), orientation.y(), orientation.z(), orientation.y());
		return orientation;
	}

	vector_3dd get_object_velocity(ncc::object::abstract_interface* obj)
	{
		if(!obj)return vector_3dd();
		vector_3dd velocity;
		obj->get_velocity(velocity.x(), velocity.y(), velocity.z());
		return velocity;			
	}


	
    scope bind_object_interface()
    {
		return class_<ncc::object::abstract_interface>("interface")
                .def("update", &object::abstract_interface::update)
				.def("add_force", &object::abstract_interface::add_force)
                .def("add_torque", &object::abstract_interface::add_torque)
                .def("add_relative_force", &object::abstract_interface::add_relative_force)
                .def("add_relative_torque", &object::abstract_interface::add_relative_torque)
                .def("set_position", &object::abstract_interface::set_position)
                .def("set_orientation", &object::abstract_interface::set_orientation)
				.def("set_velocity", &object::abstract_interface::set_velocity)
                .property("mass", &object::abstract_interface::get_mass, &object::abstract_interface::set_mass)
                .property("bounce", &object::abstract_interface::get_bounce, &object::abstract_interface::set_bounce)
                .property("friction", &object::abstract_interface::get_friction, &object::abstract_interface::set_friction)
                .def("get_id", &object::abstract_interface::get_id)
				.def("get_position", &get_object_position)
				.def("get_orientation", &get_object_orientation)
				.def("get_velocity", &get_object_velocity)
				.property("name", &object::abstract_interface::get_name, &object::abstract_interface::set_name)
				.property("id", &object::abstract_interface::get_id, &object::abstract_interface::set_id);
    }
  
	void register_script(ncc::lua::controller* script, const std::string& name, const std::string& file_name)
	{
		if(!script) return;

		ncc::controller::ptr new_script(new ncc::lua::controller(file_name, script->get_bind_function(),
																	script->osg_manager(), 
																	script->ode_manager(), 
																	script->oal_manager(),
																	script->object_manager(), 
																	script->controller_manager(),
																	script->property_manager()));
		script->controller_manager().add_prototype(name, new_script);
	}               
	ncc::controller::abstract_interface* add_controller(ncc::lua::controller* script, 
											const std::string& name, 
											const parameter_list& params)
	{
		if(!script) return 0;
		ncc::controller::weak_ptr new_controller = script->controller_manager().add_controller(name, params);
		if(ncc::controller::ptr pointer = new_controller.lock()) 
			return pointer.get();
		else 
			return 0;		
	}
        
        
	ncc::controller::abstract_interface* add_controller_as_child(ncc::lua::controller* script, 
											const std::string& name, 
											const parameter_list& params)
	{
		if(!script) return 0;
		ncc::controller::weak_ptr new_controller = script->controller_manager().add_controller(name, params, script->self());
		if(ncc::controller::ptr pointer = new_controller.lock()) 
			return pointer.get();
		else 
			return 0;		
	}
                             
    void remove_controller(ncc::lua::controller* script, const std::string& name)
	{
		if(!script) return;
		script->controller_manager().remove_controller(name);
	}
               
	void remove_controller(ncc::lua::controller* script, ncc::controller::abstract_interface* controller)
	{
		if(!script || !controller) return;
		ncc::controller::ptr controller_ptr = controller->self();
		script->controller_manager().remove_controller(controller_ptr);
	}
       
	ncc::controller::abstract_interface* find_controller(ncc::lua::controller* script, const std::string& name)
	{
		if(!script) return 0;
		ncc::controller::weak_ptr found_controller = script->controller_manager().find_controller(name);
		if(ncc::controller::ptr pointer = found_controller.lock()) 
			return pointer.get();
		else 
			return 0;		
	}            
        
    //1    
    void send_message(ncc::lua::controller* script,					
						const std::string& name,
						const parameter& message)
	{
		if(!script) return;
		script->controller_manager().send_message(script, name, message);
	}  

	//2
	void send_message(ncc::lua::controller* script,					
						const std::string& name,
						const parameter& message,
						const parameter_list& params)
	{
		if(!script) return;
		script->controller_manager().send_message(script, name, message, params);
	}
    
	//3
    void send_message(ncc::lua::controller* script,
						ncc::controller::abstract_interface* recipient,
                        const parameter& message)
	{
		if(!script || !recipient) return;
		ncc::controller::ptr controller_ptr = recipient->self();
		script->controller_manager().send_message(script, controller_ptr, message);
	}
    
	//4
    void send_message(ncc::lua::controller* script,
						ncc::controller::abstract_interface* recipient,
                        const parameter& message,
						const parameter_list& params)
	{
		if(!script || !recipient) return;
		ncc::controller::ptr controller_ptr = recipient->self();
		script->controller_manager().send_message(script, controller_ptr, message, params);
	}
        
    //5
    void send_message_to_all(ncc::lua::controller* script,
                             const std::string& type, 
                             const parameter& message)
	{
		if(!script) return;
		script->controller_manager().send_message_to_all(script, type, message);
	}
    
    //6
	void send_message_to_all(ncc::lua::controller* script,
                             const std::string& type, 
                             const parameter& message,
							 const parameter_list& params)
	{
		if(!script) return;
		script->controller_manager().send_message_to_all(script, type, message, params);
	}
        
    //7
    void send_message_to_all(ncc::lua::controller* script,                         
                             const parameter& message)
	{
		if(!script) return;
		script->controller_manager().send_message_to_all(script, message);
	}
    
    //8
	void send_message_to_all(ncc::lua::controller* script,                    
                             const parameter& message,
							 const parameter_list& params)
	{
		if(!script) return;
		script->controller_manager().send_message_to_all(script, message, params);
	}

	
	void set_gravity(ncc::lua::controller* script, double x, double y, double z)
	{
		if(!script) return;
		script->ode_manager().set_gravity(x, y, z);
	}
   
	void remove_object(ncc::lua::controller* script, ncc::object::abstract_interface* object)
	{	
		if(!script) return;
		script->object_manager().remove_object(object);
	}
	bool key_pressed(ncc::lua::controller* script,const std::string& c)
	{
		return script && !c.empty()? script->osg_manager().key_pressed(c[0]) : false;
	}
	float mouse_x(ncc::lua::controller* script)
	{
		return script ? script->osg_manager().mouse_x() : -1;
	}

	float mouse_y(ncc::lua::controller* script)
	{
		return script ? script->osg_manager().mouse_y() : -1;
	}
	bool button_pressed(ncc::lua::controller* script, int b)
	{
		return script ? script->osg_manager().button_pressed(b) : false;
	}
	void set_mouse_position(ncc::lua::controller* script, float x, float y)
	{
		if(!script) return;
		script->osg_manager().set_mouse_position(x, y);
	}
	
	struct collision_result
	{
		double depth;
		ncc::object::abstract_interface* object;
	};
	
	scope bind_collision_result()
	{
		return class_<collision_result>("collision_result")
			.def_readwrite("depth", &collision_result::depth)
			.def_readwrite("object", &collision_result::object);
	}

	
	collision_result ray_cast(ncc::lua::controller* script, 
					vector_3dd start,
					vector_3dd direction,
					double length)
	{
		if(!script)
		{
			collision_result c_result = { -1, 0};
			return c_result;
		}
		ncc::ode::object* object_hit;
		double depth = script->ode_manager().ray_cast(start.x(), start.y(), start.z(),
													 direction.x(), direction.y(), direction.z(), 
													 length, &object_hit);
													 
		collision_result c_result = { object_hit ? depth : -1, dynamic_cast<ncc::object::abstract_interface*>(object_hit)};
		return c_result;
	}
         
	osg_ode::box* create_box(ncc::lua::controller* script, 
								vector_3dd pos, 
                                vector_3dd size,
                                double mass)
    {
		if(!script) return 0;
		boost::shared_ptr<osg_ode::box> new_object(osg_ode::create_box(pos.x(), pos.y(), pos.z(), size.x(), size.y(), size.z(), mass, script->ode_manager(), script->osg_manager()));
		script->object_manager().add_object(new_object);
		return new_object.get();
    }
	
	osg_ode::sphere* create_sphere(ncc::lua::controller* script, 
									vector_3dd pos,
									double radius, 
									double mass)
    {
		if(!script) return 0;
		boost::shared_ptr<osg_ode::sphere> new_object(osg_ode::create_sphere(pos.x(), pos.y(), pos.z(),radius,mass,script->ode_manager(), script->osg_manager()));
		script->object_manager().add_object(new_object);
		return new_object.get();
    }

   	osg_ode::cylinder* create_cylinder(ncc::lua::controller* script, 
									vector_3dd pos,
									double radius, 
									double length, 
									double mass)
    {
		if(!script) return 0;
		boost::shared_ptr<osg_ode::cylinder> new_object(osg_ode::create_cylinder(pos.x(), pos.y(), pos.z(),radius,length, mass,script->ode_manager(), script->osg_manager()));
		script->object_manager().add_object(new_object);
		return new_object.get();
    }

	
	osg_ode::mesh* create_mesh(ncc::lua::controller* script, 
									const std::string& file,
									vector_3dd pos,									
									double mass)
    {
		if(!script) return 0;
		boost::shared_ptr<osg_ode::mesh> new_object(osg_ode::create_mesh(file, pos.x(), pos.y(), pos.z(), mass, script->ode_manager(), script->osg_manager()));
		script->object_manager().add_object(new_object);
		return new_object.get();
    }
		osg_ode::invisible_capsule* create_invisible_capsule(ncc::lua::controller* script, 
									vector_3dd pos,
									double radius,
									double length, 
									double mass)
    {
		if(!script) return 0;
		boost::shared_ptr<osg_ode::invisible_capsule> new_object(osg_ode::create_invisible_capsule(pos.x(), pos.y(), pos.z(),radius, length, mass,script->ode_manager()));
		script->object_manager().add_object(new_object);
		return new_object.get();
    }

	template <class property_type>
	property_type get_property(ncc::lua::controller* script, const std::string& index)
	{
		if(!script) return property_type();
		parameter prop = script->property_manager().get_property(index);
		return !prop.empty() ? get<property_type>(prop) : property_type();
	}
	template <class property_type>
	void set_property(ncc::lua::controller* script, const std::string& index, const property_type& property)
	{
		if(!script) return;
		script->property_manager().set_property(index, parameter(property));
	}
	
	vector_3dd get_camera_position(ncc::lua::controller* script)
	{
		if(!script) return vector_3dd();
		vector_3dd camera_pos;
		script->osg_manager().get_camera_position(camera_pos.x(), camera_pos.y(), camera_pos.z());
		return camera_pos;
	}
	quaterniond get_camera_orientation(ncc::lua::controller* script)
	{
		if(!script) return quaterniond();
		quaterniond orn;
		script->osg_manager().get_camera_orientation(orn.x(), orn.y(), orn.z(), orn.w());
		return orn;
	}
	
	void camera_look_at(ncc::lua::controller* script,
						const vector_3dd& camera_pos, const vector_3dd& obj_pos)
	{
		if(!script) return;
		script->osg_manager().look_at(camera_pos.x(), camera_pos.y(), camera_pos.z(),
									  obj_pos.x(), obj_pos.y(), obj_pos.z());
	}

	void register_sound(ncc::lua::controller* script, const std::string& file, const std::string& name)
	{
		if(!script) return;
		script->oal_manager().register_sound(file, name);
	}
	int play_sound(ncc::lua::controller* script, const std::string& name, double volume, bool loop)
	{
		if(!script) return -1;
		return script->oal_manager().play(name, volume, loop);
	}
	void stop_sound(ncc::lua::controller* script, int sound_id)
	{
		if(!script) return;
		script->oal_manager().stop(sound_id);
	}
	void clear_sounds(ncc::lua::controller* script)
	{
		if(!script) return;
		script->oal_manager().flush();
	}
	
    scope bind_script_controller()
    {
		//here, controller is ncc::lua::controller because it is in the ncc::lua namespace
		return class_<ncc::lua::controller, ncc::controller::abstract_interface>("controller")                       
			.def("remove_self", &ncc::lua::controller::remove)			
			.def("remove_object", &remove_object)
			.def("key_pressed", &key_pressed)
			.def("get_mouse_x", &mouse_x)
			.def("get_mouse_y", &mouse_y)
			.def("set_mouse_position", &set_mouse_position)
			.def("button_pressed", &button_pressed)
			.def("register_script", &register_script)
			.def("add_controller", &add_controller)
			.def("add_controller_as_child", &add_controller_as_child)
			.def("remove_controller", (void(*)(ncc::lua::controller*, const std::string&))&remove_controller)
			.def("remove_controller", (void(*)(ncc::lua::controller*, ncc::controller::abstract_interface*))&remove_controller)
			.def("find_controller", &find_controller)
			.def("set_collision_callback", &ncc::lua::controller::set_collision_callback)
			.def("send_message", (void(*)(ncc::lua::controller*,const std::string&,const parameter&))&send_message)
			.def("send_message", (void(*)(ncc::lua::controller*,const std::string&,const parameter&, const parameter_list&))&send_message)
			.def("send_message_to", (void(*)(ncc::lua::controller*,ncc::controller::abstract_interface*,const parameter&))&send_message)
			.def("send_message_to", (void(*)(ncc::lua::controller*,ncc::controller::abstract_interface*,const parameter&, const parameter_list&))&send_message)
			.def("send_message_to_all", (void(*)(ncc::lua::controller*,const std::string&,const parameter&))&send_message_to_all)
			.def("send_message_to_all", (void(*)(ncc::lua::controller*,const std::string&,const parameter&, const parameter_list&))&send_message_to_all)
			.def("send_message_to_all", (void(*)(ncc::lua::controller*,const parameter&))&send_message_to_all)
			.def("send_message_to_all", (void(*)(ncc::lua::controller*,const parameter&, const parameter_list&))&send_message_to_all)
			.def("set_gravity", &set_gravity)
			.def("ray_cast", &ray_cast)
			.def("get_camera_position", &get_camera_position)
			.def("get_camera_orientation", &get_camera_orientation)
			.def("look_at", &camera_look_at)
			.def("get_int_property", &get_property<int>)
			.def("set_int_property", &set_property<int>)
			.def("get_double_property", &get_property<double>)
			.def("set_double_property", &set_property<double>)
			.def("get_vector_property", &get_property<vector_3dd>)
			.def("set_vector_property", &set_property<vector_3dd>)
			.def("get_string_property", &get_property<std::string>)
			.def("set_string_property", &set_property<std::string>)
			.def("get_object_property", &get_property<ncc::object::abstract_interface*>)
			.def("set_object_property", &set_property<ncc::object::abstract_interface*>)
			.def("create_box", &create_box)
			.def("create_sphere", &create_sphere)
			.def("create_cylinder", &create_cylinder)
			.def("create_mesh", &create_mesh)
			.def("create_invisible_capsule", &create_invisible_capsule)
			.def("register_sound", &register_sound)
			.def("play_sound", &play_sound)
			.def("stop_sound", &stop_sound)
			.def("clear_sounds", &clear_sounds);
    }
	

    scope bind_controller_interface()
    {        
		return class_<ncc::controller::abstract_interface>("interface")
            .def("get_type", &ncc::controller::abstract_interface::get_type)
            .def("get_type_id", &ncc::controller::abstract_interface::get_type)
            .def("handle_message", &ncc::controller::abstract_interface::handle_message)
            .def("initialize", &ncc::controller::abstract_interface::initialize)
            .def("get_name", &ncc::controller::abstract_interface::get_name)
            .def("set_name", &ncc::controller::abstract_interface::set_name, dependency(result, _1))
            .def("control", &ncc::controller::abstract_interface::control)
            .def("is_alive",&ncc::controller::abstract_interface::is_alive);
    }       	
    scope bind_osg_ode_mesh()
    {
        using namespace osg_ode;
		return class_<mesh, bases<osg::object, ode::object, object::abstract_interface> >("mesh")
			.def("load_texture", &box::load_texture);
    }

    scope bind_osg_ode_sphere()
    {
        using namespace osg_ode;
		return class_<sphere,  bases<osg::object, ode::object, object::abstract_interface> >("sphere")
			.def("load_texture", &box::load_texture);
    }
	   
    scope bind_osg_ode_cylinder()
    {
        using namespace osg_ode;
		return class_<cylinder,  bases<osg::object, ode::object, object::abstract_interface> >("cylinder")
			.def("load_texture", &box::load_texture);
    }
   
	scope bind_osg_ode_box()
    {
        using namespace osg_ode;
		return class_<box,  bases<osg::object, ode::object, object::abstract_interface> >("box")
			.def("load_texture", &box::load_texture);
    }
	scope bind_invisible_capsule()
	{
		using namespace osg_ode;
		return class_<invisible_capsule, bases<ode::object, object::abstract_interface> >("invisible_capsule");
	}
   	      
	template <class type>
	bool parameter_is_type(parameter_list* params, int index)
	{		
		return params ? is_type<type>(params->at(index)) : false;
	}

	template <class type>
	type get_parameter(parameter_list* params, int index)
	{			
		return params ? ncc::get<type>(params->at(index)) : type();
	}

	int get_parameter_size(parameter_list* params)
	{
		return params ? params->size() : 0;
	}		  

	scope bind_parameter_list()
	{
		 return class_<parameter_list >("parameter_list")				
				.def("get_int", (int(*)(parameter_list*, int))&get_parameter<int>)
				.def("get_double", (double(*)(parameter_list*, int))&get_parameter<double>)
				.def("get_string", (std::string(*)(parameter_list*, int))&get_parameter<std::string>)
				.def("get_vector", (vector_3dd(*)(parameter_list*, int))&get_parameter<vector_3dd>)
				.def("get_object", (ncc::object::abstract_interface*(*)(parameter_list*, int))&get_parameter<ncc::object::abstract_interface*>)
				.def("get_controller",(ncc::controller::abstract_interface*(*)(parameter_list*, int))&get_parameter<ncc::controller::abstract_interface*>)
				.def("size", &get_parameter_size)
				.def("is_int", &parameter_is_type<int>)
				.def("is_double", &parameter_is_type<double>)
				.def("is_string", &parameter_is_type<const char*>)
				.def("is_vector", &parameter_is_type<vector_3dd>)
				.def("is_object", &parameter_is_type<ncc::object::abstract_interface*>)
				.def("is_controller", &parameter_is_type<ncc::controller::abstract_interface*>);
	}

	template <class type>
	type get_single_parameter(parameter* param)
	{
		return param ? ncc::get<type>(*param): type();
	}
	template <class type>
	bool single_parameter_is_type(parameter* param)
	{
		return param ? ncc::is_type<type>(*param) : false;
	}

	scope bind_parameter()
	{
		return class_<parameter>("parameter")
				.def(constructor<>())
				.def(constructor<const std::string&>())
				.def(constructor<const double&>())
				.def(constructor<const vector_3dd&>())
				.def(constructor<const ncc::object::abstract_interface*>())
				.def(constructor<const ncc::controller::abstract_interface*>())
				.def(constructor<const parameter&>())
				.def("get_int", &get_single_parameter<int>)
				.def("get_double", &get_single_parameter<double>)
				.def("get_string", &get_single_parameter<std::string>)
				.def("get_vector", &get_single_parameter<vector_3dd>)
				.def("get_object", &get_single_parameter<ncc::object::abstract_interface*>)
				.def("get_controller", &get_single_parameter<ncc::controller::abstract_interface*>)
				.def("is_int", &single_parameter_is_type<int>)
				.def("is_double", &single_parameter_is_type<double>)
				.def("is_string", &single_parameter_is_type<std::string>)
				.def("is_vector", &single_parameter_is_type<vector_3dd>)
				.def("is_object", &single_parameter_is_type<ncc::object::abstract_interface*>)
				.def("is_controller", &single_parameter_is_type<ncc::controller::abstract_interface*>);
	}

	char myrand()
	{
		static bool init = false;
		if(!init)
		{
			srand(time(0));
			init = true;
		}
		return static_cast<char>((rand() / static_cast<double>(RAND_MAX) * 0.2) * 0.1);
	}

    void bind_ncc(lua_State* state)
    {
        module(state, "ncc")
        [			
            bind_vector_3dd(),
			bind_quaterniond(),
			def("slerp", quaterniond::slerp),
			def("lerp", quaterniond::lerp),
			def("rotate_to", quaterniond::rotate_to),
            namespace_("controller")
            [
                bind_controller_interface()               
            ],
            namespace_("object")
            [
                bind_object_interface()
            ],			
            namespace_("osg_ode")
            [
				class_<ode::object>("ode_object"),
				class_<osg::object>("osg_object"),
                bind_osg_ode_mesh(),
                bind_osg_ode_sphere(),
                bind_osg_ode_cylinder(),
                bind_osg_ode_box(),
				bind_invisible_capsule()				
            ],
			namespace_("script")
			[
				bind_script_controller()
			],
			bind_parameter(),
			bind_parameter_list(),
			bind_collision_result(),
            def("parameters", (parameter_list(*)(const parameter&))&parameters<parameter>),
            def("parameters", (parameter_list(*)(const parameter&, const parameter&))&parameters<parameter, parameter>),
            def("parameters", (parameter_list(*)(const parameter&, const parameter&, const parameter&))&parameters<parameter, parameter, parameter>),
            def("parameters", (parameter_list(*)(const parameter&, const parameter&, const parameter&, const parameter&))&parameters<parameter, parameter, parameter, parameter>),
            def("parameters", (parameter_list(*)(const parameter&, const parameter&, const parameter&, const parameter&, const parameter&))&parameters<parameter, parameter, parameter, parameter, parameter>),
            def("parameters", (parameter_list(*)(const parameter&, const parameter&, const parameter&, const parameter&, const parameter&, const parameter&))&parameters<parameter, parameter, parameter, parameter, parameter, parameter>),
            def("parameters", (parameter_list(*)(const parameter&, const parameter&, const parameter&, const parameter&, const parameter&, const parameter&, const parameter&))&parameters<parameter, parameter, parameter, parameter, parameter, parameter, parameter>),
            def("parameters", (parameter_list(*)(const parameter&, const parameter&, const parameter&, const parameter&, const parameter&, const parameter&, const parameter&, const parameter&))&parameters<parameter, parameter, parameter, parameter, parameter, parameter, parameter, parameter>),
            def("parameters", (parameter_list(*)(const parameter&, const parameter&, const parameter&, const parameter&, const parameter&, const parameter&, const parameter&, const parameter&, const parameter&))&parameters<parameter, parameter, parameter, parameter, parameter, parameter, parameter, parameter, parameter>),
            def("parameters", (parameter_list(*)(const parameter&, const parameter&, const parameter&, const parameter&, const parameter&, const parameter&, const parameter&, const parameter&, const parameter&, const parameter&))&parameters<parameter, parameter, parameter, parameter, parameter, parameter, parameter, parameter, parameter, parameter>),
			def("int_parameter", &create<int>),
			def("double_parameter", &create<double>),
			def("string_parameter", &create<std::string>),
			def("vector_parameter", &create<vector_3dd>),
			def("rand", &myrand)
        ];
    }

}//namespace utilities
}//namespace lua
}//namespace ncc
