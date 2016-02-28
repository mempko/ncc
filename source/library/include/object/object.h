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

#ifndef NCCENTRIFUGE_OBJECT_H
#define NCCENTRIFUGE_OBJECT_H
 
#include "object/object_interface.h"
///@defgroup physics Physics
///These are classes which are related to physics.

///@defgroup physics_policy_classes Physics Policy Classes
///These are classes which satisfy the ncc::object physics_body template 
///paramater.
///
///These are policy classes which can be used to specify the physical_body 
///template paramater of ncc::object.
///@see ncc::object For more information on using these policy classes.

///@defgroup managers Managers
///Managers handle various resources.

///@defgroup opengl OpenGL
///These are classes which use OpenGL.

///@defgroup visual_policy_classes Visual Policy Classes
///These are classes which statisfy the ncc::object visual_body template 
///parameter.
///
///These are policy classes which can be used to specify the visual_body 
///template parameter of ncc::object.
///@see ncc::object for more information on using these policy classes.

///@defgroup objects 3D Objects
///These are objects which you can see an manipulate.

namespace ncc {
namespace object 
{
    ///Template class which represents 3d game objects
    ///
    ///This template class represents a 3d object in a game. It uses
    ///two template parameters to define its visual body and physical body. For 
    ///example, I can have the visual shape be a ncc::osg::box 
    ///(@ref visual_policy_classes "Some classes which can be used for the 
    ///visual_body template paramater") and the physical body to be ncc::ode::box 
    ///(@ref physical_policy_classes "Some classes which can be used for the 
    ///physical_body template paramater").
    ///\n\n
    ///Example:
    ///     - ncc::object<ncc::osg::box, ncc::ode::box> my_box; \n
    ///       creates an object which uses OpenSceneGraph to render a box\n
    ///       and OpenDynamicsEngine (ODE) for physics simulation.
    ///
    ///\n\n
    ///There are some default policies defined for OpenSceneGraph and ODE. The 
    ///template class uses a pattern known as
    ///the policy pattern. This uses policy classes to define specific behavior. 
    ///This particular class uses two policies to define the physical behaivior and 
    ///the visual behavior. The reason for using policies is that it allowes 
    ///creating classes in a more flexible combinatory way. So using the predefined 
    ///policies you can create many different type of 3d objects. You can have a 
    ///mesh object which loads from a file but has a physical shape of a shere in 
    ///the collision world. See the typdefed objects such as osg_ode::box and 
    ///osg_ode::mesh and their corresponding create functions to see the power of 
    ///templates and policies.
    ///\n\n
    ///Required visual_body policy methods to fullfill the needed visual abstract_interface. 
    ///See ncc::osg::object for an example of a class which fullfills this
    ///abstract_interface.
    ///     - void update_orientation(double x, double y, double z, double w) 
    ///     - void update_position(double x, double y, double z)
    ///     - void update() 
    ///
    ///\n\n
    ///Required physical_body policy methods to fullfill the needed physical 
    ///abstract_interface. See ncc::ode::object for an example of a class which fullfills 
    ///this abstract_interface.
    ///     - void add_force(double x, double y, double z) 
    ///     - void add_torque(double x, double y, double z)
    ///     - void add_relative_force(double x, double y, double z) 
    ///     - void add_relative_torque(double x, double y, double z)
    ///     - void get_orientation(double& x, double& y, double& z, double& w)
    ///     - void get_position(double& x, double& y, double& z) 
    ///     - void update() 
    ///     - virtual void set_mass(double x)
    ///     - virtual double get_mass() const
    ///     - virtual void set_bounce(double bounce) 
    ///     - virtual double get_bounce() const 
    ///     - virtual void set_friction(double friction)
    ///     - virtual double get_friction() const 
    ///@ingroup objects
    template <class visual_body, class physical_body>  
        class object : public abstract_interface, public visual_body, public physical_body
    {
        public:
            virtual void update();
            virtual void add_force(double x, double y, double z) { physical_body::add_force(x, y, z);}
            virtual void add_torque(double x, double y, double z) {physical_body::add_torque(x, y, z);}
            virtual void add_relative_force(double x, double y, double z) { physical_body::add_relative_force(x, y, z);}
            virtual void add_relative_torque(double x, double y, double z) {physical_body::add_relative_torque(x, y, z);}
            virtual void set_position(double x, double y, double z) {physical_body::set_position(x, y, z);}
            virtual void get_position(double& x, double& y, double& z) const { physical_body::get_position(x, y, z);}
            virtual void set_orientation(double x, double y, double z, double w) {physical_body::set_orientation(x, y, z, w);}
            virtual void get_orientation(double& x, double& y, double& z, double& w) const {physical_body::get_orientation(x, y, z, w);}
            virtual void set_velocity(double x, double y, double z) {physical_body::set_velocity(x, y, z);}
            virtual void get_velocity(double& x, double& y, double& z) const { physical_body::get_velocity(x, y, z);}

            virtual void set_mass(double x) {physical_body::set_mass(x);}
            virtual double get_mass() const {return physical_body::get_mass();}
            virtual void set_bounce(double bounce) {physical_body::set_bounce(bounce);}
            virtual double get_bounce() const {return physical_body::get_bounce();}
            virtual void set_friction(double friction) {physical_body::set_friction(friction);}
            virtual double get_friction() const {return physical_body::get_friction();}

            object(): visual_body(), physical_body(), abstract_interface() {}        
    };

    template <class visual_body, class physical_body> 
        void 
        object<visual_body, physical_body>::update()
        {
            //update the physical body
            physical_body::update();
            //get the physical body orientation and update the visual body orientation
            double quat[4];
            physical_body::get_orientation(quat[0], quat[1], quat[2], quat[3]);
            visual_body::update_orientation(quat[0], quat[1], quat[2], quat[3]);

            //get the physical body position and update the visual body position
            physical_body::get_position(quat[0], quat[1], quat[2]);
            visual_body::update_position(quat[0], quat[1], quat[2]);

            //update the visual body
            visual_body::update();
        }

    template <class list>
        void update_objects(list& objects)
        {
            typename list::iterator end = objects.end(); 
            for (typename list::iterator object = objects.begin(); object != end; ++ object)
                (*object)->update(); //call the box update function
        }
}//namespace object
}//namespace ncc
#endif

