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

#ifndef NCCENTRIFUGE_ODE_MANAGER_H
#define NCCENTRIFUGE_ODE_MANAGER_H

#include <algorithm>
#include <boost/utility.hpp>
#include <boost/shared_array.hpp>
#include <ode/ode.h>
#include <boost/function.hpp>
#include "utilities/cache.h"
#include "utilities/debug.h"
#include "object/object_interface.h"

namespace ncc {
namespace ode
{
    struct trimesh_data
    {
        int vertex_count;
        boost::shared_array<dVector3> vertices;
        boost::shared_array<dTriIndex> indices;
        dTriMeshDataID data_id;
        ~trimesh_data() {if(data_id) dGeomTriMeshDataDestroy(data_id);}
    };
    typedef cache<trimesh_data> trimesh_data_cache;
    class object;

    struct collision_info
    {
        double depth;
        ncc::object::abstract_interface* object_1;
        ncc::object::abstract_interface* object_2;
    };

    typedef boost::function<bool (const collision_info)> collision_callback;

    ///Manages ODE physics objects. 
    ///
    ///Manages various aspects of objects using the ODE library. All objects
    ///derived from ncc::ode::object require the manager to be created. Use the 
    ///step method to step the physics simulations by a certian time step. 
    ///@ingroup managers
    ///@ingroup physics
    class manager : boost::noncopyable
    {
        public:
            ///used by ncc::ode::trimesh.
            typedef boost::shared_ptr<trimesh_data> trimesh_data_ptr; 

            manager(double erp= 0.2, double cfm = 1e-5);
            ~manager();

            ///Steps the physics simulation by a certain timestep
            ///
            ///This method steps the physics simulation by a certain timestep. 
            ///The smaller the timestep the more accurate the physics 
            ///simulation...but the more slow the physics.
            void step(double step_size);

            ///Sets the gravity vector for the physics world.
            void set_gravity(double x, double y, double z) { dWorldSetGravity (world_id, x ,y,z);}      

            ///Returns the ode space all objects are in.
            dSpaceID ode_space() { return space_id;}

            ///Returns the ode world all objects are in.
            dWorldID ode_world() { return world_id;}

            ///Returns the ode contact group to use with collision.
            dJointGroupID contact_group() { return contact_group_id;}

            double ray_cast(double origin_x, double origin_y, double origin_z, 
                    double direction_x, double direction_y, double direction_z, 
                    double length, ode::object** obj);
            //
            ///Returns a refrence to the trimesh_cache
            ///@{
            trimesh_data_cache& trimesh_cache() {return mesh_cache;}
            const trimesh_data_cache& trimesh_cache() const {return mesh_cache;}
            ///@}
        private:          
            double ERP;// = 0.999;

            /// ODE's "Constraint Force Mixing". This is empirically defined.
            double CFM;// = 1e-10;

            /// The ODE world.
            dWorldID world_id;

            /// Joint group for contacts (for collision detection).
            dJointGroupID contact_group_id;

            /// ODE space for collision detection.
            dSpaceID space_id;

            trimesh_data_cache mesh_cache;

    };
} //namespace ode
} //namespace ncc
#endif

