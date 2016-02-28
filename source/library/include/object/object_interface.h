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

#ifndef NCCENTRIFUGE_OBJECT_INTERFACE_H
#define NCCENTRIFUGE_OBJECT_INTERFACE_H

#include <list>
#include <string>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include "elements/id.h"
namespace ncc {
namespace object
{
    ///An abstract_interface class for general 3d object control
    ///
    ///All physical objects in a 3d enviornment will have this abstract_interface.
    ///@ingroup objects
    ///@ingroup interfaces
    class abstract_interface : public id_type
    {
        public:
            ///Updates the visual representation. 
            ///
            ///An object normally has some internal state such as position and 
            ///orientation. This function is called whenever an update on the object 
            ///is needed. For instance this function may update the visual look of 
            ///an object or animation. 
            virtual void update() = 0;

            ///Adds force in world space to the object
            ///
            ///This will add a force to the center of mass of an object in world 
            ///space the three parameters are x, y, and z components of the force 
            ///vector.
            virtual void add_force(double x, double y, double z) = 0;

            ///Adds torque in worlds space to the object
            ///
            ///This will add torque at the center of mass of an object in world 
            ///space the three parameters are x, y, and z components of the torque 
            ///vector.
            virtual void add_torque(double x, double y, double z) = 0;

            ///Adds a force to an object in object space.
            ///
            ///Adds a force relative to the objects center of mass. 
            ///the three parameters are x, y, and z components of the force vector.
            virtual void add_relative_force(double x, double y, double z) = 0;
            ///Adds a torque to an object in object space.
            ///
            ///Adds a torque relative to the objects center of mass.
            ///the three parameters are x, y, and z components of the torque vector.
            virtual void add_relative_torque(double, double, double) = 0;

            ///Sets the position of the object in world space.
            virtual void set_position(double x, double y, double z) = 0;

            ///Returns the position fo the object.
            ///
            ///Returns the position of the object as a vector using the 3 reference 
            ///paramaters x, y, and z.
            virtual void get_position(double& x, double& y, double& z) const = 0;

            ///Returns the orientation of the object.
            ///
            ///Returns the orientation of the object as a quaternion using the 4 
            ///reference paramaters x, y, z, and w.
            ///x, y, and z represent the vector component and w is the scalar part 
            ///of the quaternion.
            virtual void get_orientation(double& x, double& y, double& z, double& w) const = 0;

            ///Sets the orientation of the object in world space. 
            ///
            ///The orientation is in quaternion format where x, y, and z are the 
            ///vector component and w is the scalar part.
            virtual void set_orientation(double y, double x, double z, double w) = 0;

            ///Returns the position of the object.
            ///
            ///Returns the velocity of the object using three x, y, and z reference 
            ///parameters.
            virtual void get_velocity(double& x, double& y, double& z) const = 0;

            ///Sets the velocity of the object in world space.
            ///
            ///The velocity of the object is specified by the three x, y, and z
            ///parameters. These parameters specify a vector where the direction
            ///is the direction of the velocity and the length is the speed.
            virtual void set_velocity(double x, double y, double z) = 0;

            ///Sets the mass of the object.
            virtual void set_mass(double mass) = 0;

            ///Returns the mass of the object.
            virtual double get_mass() const = 0;

            ///Sets the bouncyness of the object
            ///
            ///Sets the bouncyness of an object. The range is [0,1] where 0 is no 
            ///bounce and 1 is maximum bounce.
            virtual void set_bounce(double bounce) = 0;

            ///Returns the bouncyness of an object
            virtual double get_bounce() const = 0;

            ///Sets the friction of the objects surface
            ///
            ///Sets the friction of the object. 0 means no friction while -1 means 
            ///infinite friction.
            virtual void set_friction(double friction) = 0;

            ///Returns the friction of the objects surface
            virtual double get_friction() const = 0;

            abstract_interface() : id_type() {}
            virtual ~abstract_interface(){}

            const std::string& get_name() const { return name;}
            void set_name(const std::string& value) { name = value;}

            const int& get_id() const { return id_value;}
            void set_id(const int& value) { id_value = value;}
        private:
            std::string name;
            int id_value;
    };

    typedef boost::shared_ptr<abstract_interface> ptr;
    typedef boost::weak_ptr<abstract_interface> weak_ptr;
    typedef std::list<ptr > list;
    typedef std::list<weak_ptr> weak_list;

}//namespace object
}//namespace ncc
#endif

