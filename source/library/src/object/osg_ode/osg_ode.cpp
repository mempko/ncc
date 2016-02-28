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

#include "object/osg_ode/osg_ode.h"
namespace ncc {
namespace osg_ode
{

   //ghost object creation functions
    ghost_box* create_ghost_box(
                            double x,
                            double y,
                            double z,
                            double size_x,
                            double size_y,
                            double size_z,
                            osg::manager& osg_manager)
    {
        ghost_box* obj = new ghost_box;
        obj->create_visual_body(size_x, size_y, size_z, osg_manager);
        obj->create_physical_body(x, y, z);
        return obj;
    }

    ghost_sphere* create_ghost_sphere(
                            double x,
                            double y,
                            double z,
                            double radius,
                            osg::manager& osg_manager)
    {
        ghost_sphere* obj = new ghost_sphere;
        obj->create_visual_body(radius, osg_manager);
        obj->create_physical_body(x, y, z);
        return obj;
    }

    ghost_mesh* create_ghost_mesh(
                            const std::string& file_name,
                            double x,
                            double y,
                            double z,
                            osg::manager& osg_manager)
    {
        ghost_mesh* obj = new ghost_mesh;
        obj->create_visual_body(file_name, osg_manager);
        obj->create_physical_body(x, y, z);
        return obj;
    }


    box* create_box(
                            double x,
                            double y,
                            double z,
                            double size_x,
                            double size_y,
                            double size_z,
                            double mass,
                            ode::manager& ode_manager,
                            osg::manager& osg_manager)
    {
        box* obj = new box;
        obj->create_visual_body(size_x, size_y, size_z, osg_manager);
        obj->create_physical_body(x, y, z, size_x, size_y, size_z, mass, ode_manager);
        return obj;
    }
                                        
    sphere* create_sphere(
                                    double x,
                                    double y,
                                    double z,
                                    double radius,
                                    double mass,
                                    ode::manager& ode_manager,
                                    osg::manager& osg_manager)
    {
        sphere* obj = new sphere;
        obj->create_visual_body(radius, osg_manager);
        obj->create_physical_body(x, y, z, radius, mass,ode_manager);
        return obj;
    }

    cylinder* create_cylinder(
                                    double x,
                                    double y,
                                    double z,
                                    double radius,
                                    double length,
                                    double mass,
                                    ode::manager& ode_manager,
                                    osg::manager& osg_manager)
    {
        cylinder* obj = new cylinder;
        obj->create_visual_body(radius, length, osg_manager);
        obj->create_physical_body(x, y, z, radius, length, mass,ode_manager);
        return obj;
    }

    mesh* create_mesh(
                                const std::string& file_name,
                                double x,
                                double y,
                                double z,
                                double mass,
                                ode::manager& ode_manager,
                                osg::manager& osg_manager)
    {
        mesh* obj = new mesh;
        
        boost::progress_display progress(4, std::cout);

        obj->create_visual_body(file_name, osg_manager);
        ++progress;
        
        std::vector<double> vertices;
        std::vector<int> indices;
        obj->get_trimesh_data(vertices, indices);
        ++progress;

        double size_x, size_y, size_z, center_x, center_y, center_z;
        obj->get_bounding_box(size_x, size_y, size_z, center_x, center_y, center_z);
        ++progress;

        obj->create_physical_body(x, y, z, size_x, size_y, size_z, mass, vertices, indices, file_name, ode_manager);
        ++progress;

        return obj;
    }
    
      box_bound_mesh* create_box_bound_mesh(
                            const std::string& file_name,
                            double x,
                            double y,
                            double z,
                            double mass,
                            ode::manager& ode_manager,
                            osg::manager& osg_manager)
        {
            box_bound_mesh* obj = new box_bound_mesh;
            obj->create_visual_body(file_name, osg_manager);
            
            double size_x, size_y, size_z, center_x, center_y, center_z;
            obj->get_bounding_box(size_x, size_y, size_z, center_x, center_y, center_z);
            
            obj->create_physical_body(x, y, z, size_x, size_y, size_z, mass, ode_manager);
            return obj;
        }
        capsule_mesh* create_capsule_mesh(
                            const std::string& file_name,
                            double x, 
                            double y,
                            double z,
                            double radius,
                            double length,
                            double mass,
                            ode::manager& ode_manager,
                            osg::manager& osg_manager)
        {
            capsule_mesh* obj = new capsule_mesh;
            obj->create_visual_body(file_name, osg_manager);
            obj->create_physical_body(x, y, z, radius, length, mass, ode_manager);
            return obj;
        }
		
		invisible_capsule* create_invisible_capsule(double x, 
											   double y, 
											   double z, 
											   double radius,
											   double length,
											   double mass,
											   ode::manager& ode_manager)
		{
			invisible_capsule* obj = new invisible_capsule;
			obj->create_physical_body(x, y, z, radius, length, mass, ode_manager);
			return obj;
		}

}//namespace osg_ode
}//namespace ncc
