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

#ifndef NCCENTRIFUGE_OBJECT_UTILITIES
#define NCCENTRIFUGE_OBJECT_UTILITIES
#include "utilities/vector_3d.h"
#include "utilities/quaternion.h"
namespace ncc {
namespace object 
{
    ///Ghost object is one which moves in a direction at a certain velocity.
    ///
    ///A ghost does not do any coliision detection. It simply continues on its 
    ///course at a certain velocity.
    class ghost
    {
        public:
            ghost() : position(), velocity(), orientation() {}
            ghost(const ghost& other) : position(other.position), velocity(other.velocity), orientation(other.orientation) {}
            void create_physical_body(double x, double y, double z);
            void update();
            void add_force(double x, double y, double z);
            void add_torque(double x, double y, double z);
            void add_relative_force(double x, double y, double z);
            void add_relative_torque(double x, double y, double z);
            void set_position(double x, double y, double z);
            void get_position(double& x, double& y, double& z) const;
            void set_orientation(double x, double y, double z, double w);
            void get_orientation(double& x, double& y, double& z, double& w) const;
            void get_velocity(double& x, double& y, double& z) const;
            void set_velocity(double x, double y, double z);
            void point_to(double& x, double& y, double& z);
            void rotate_by(double& x, double& y, double& z);
            void set_mass(double x){};
            double get_mass() const {return 0.0;}
            void set_bounce(double bounce){}
            double get_bounce() const{return 0.0;}
            void set_friction(double friction){}
            double get_friction() const {return 0.0;}
        protected:
            vector_3dd position;
            vector_3dd velocity;
            quaterniond orientation;
    };

    class invisible
    {
        public:
            void update_orientation(double x, double y, double z, double w) {}
            void update_position(double x, double y, double z) {}
            void update(){};
    };
}//namespace simple
}//namespace ncc
#endif

