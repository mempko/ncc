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

#include "object/object_utilities.h"
namespace ncc {
namespace object {

    void ghost::create_physical_body(double x, double y, double z)
    {
        position.set(x, y, z);
    }

    void ghost::update()
    {
        position += velocity;
    }
    void ghost::add_force(double x, double y, double z)
    {
        velocity += vector_3dd(x, y, z);
    }
    void ghost::add_torque(double x, double y, double z)
    {
        rotate_by(x, y, z);
    }
    void ghost::add_relative_force(double x, double y, double z)
    {
        vector_3dd world_velocity_offset = orientation * vector_3dd(x,y,z);
        velocity += world_velocity_offset;
    }
    void ghost::add_relative_torque(double x, double y, double z)
    {
        quaterniond local_rotation;
        local_rotation.create_from_euler(x, y, z);
        orientation = local_rotation * orientation;
    }
    void ghost::set_position(double x, double y, double z)
    {
        position.set(x, y, z);
    }
    void ghost::get_position(double& x, double& y, double& z) const
    {
        x = position.x(); y = position.y(); z = position.z();
    }
    void ghost::set_orientation(double x, double y, double z, double w)
    {
        orientation.set(x, y, z, w);
    }
    void ghost::get_orientation(double& x, double& y, double& z, double& w) const
    {
        x = orientation.x(); y = orientation.y(); z = orientation.z(); w = orientation.w();       
    }
	
	void ghost::get_velocity(double& x, double& y, double& z) const
	{
		x = velocity.x(); y = velocity.y(); z = velocity.z();
	}
	void ghost::set_velocity(double x, double y, double z)
	{
		velocity.set(x, y, z);
	}
    void ghost::point_to(double& x, double& y, double& z)
    {
        orientation = quaterniond::rotate_to(vector_3dd(1,0,0), vector_3dd(x, y, z) - position);    
    }
    void ghost::rotate_by(double& x, double& y, double& z)
    {
        quaterniond rotation;
        rotation.create_from_euler(x, y, z);
        orientation *= rotation;
    }


}//namespace object
}//namespace ncc
