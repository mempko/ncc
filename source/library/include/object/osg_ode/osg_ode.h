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

#ifndef NCCENTRIFUGE_OSG_ODE_H
#define NCCENTRIFUGE_OSG_ODE_H
#include "object/osg/osg_policies.h"
#include "object/ode/ode_policies.h"
#include "object/object_utilities.h"
#include "object/object.h"
namespace ncc {
namespace osg_ode 
{
    ///@addtogroup objects
    ///@{

    //ghost objects

    ///A box which can go through walls.
    typedef object::object<osg::box, object::ghost> ghost_box;
    typedef boost::shared_ptr<ghost_box> ghost_box_ptr;

    ///A sphere which can go through walls.
    typedef object::object<osg::sphere, object::ghost> ghost_sphere;
    typedef boost::shared_ptr<ghost_sphere> ghost_sphere_ptr;


    ///A mesh which can go through walls.
    typedef object::object<osg::mesh, object::ghost> ghost_mesh;
    typedef boost::shared_ptr<ghost_mesh> ghost_mesh_ptr;


    //physical objects

    ///A rigid body box.
    typedef object::object<osg::box, ode::box> box;
    typedef boost::shared_ptr<box> box_ptr;


    ///A rigid body sphere.
    typedef object::object<osg::sphere, ode::sphere> sphere;
    typedef boost::shared_ptr<sphere> sphere_ptr;

    ///A rigid body sphere.
    typedef object::object<osg::cylinder, ode::cylinder> cylinder;
    typedef boost::shared_ptr<cylinder> cylinder_ptr;

    ///A rigid body mesh.
    typedef object::object<osg::mesh, ode::trimesh> mesh;
    typedef boost::shared_ptr<mesh> mesh_ptr;

    ///A mesh bound by a rigid body box.
    typedef object::object<osg::mesh, ode::box> box_bound_mesh;
    typedef boost::shared_ptr<box_bound_mesh> box_bound_mesh_ptr;


    ///A mesh bound by a capsule.
    typedef object::object<osg::mesh, ode::capsule> capsule_mesh;
    typedef boost::shared_ptr<capsule_mesh> capsule_mesh_ptr;


    ///an invisible capsule
    typedef object::object<object::invisible, ode::capsule> invisible_capsule;
    typedef boost::shared_ptr<invisible_capsule> invisible_capsule_ptr;


    ///This is an example of creating a box that will uses OpenSceneGraph to 
    ///render an ODE to do the physics. In this example the box will simply fall 
    ///to infinity. If you compile this program you will see a white box fall in 
    ///front of you.
    ///@example create_object.cpp

    ///This is an example of using controllers to control a 3d box. As with the 
    ///create_object.cpp example, the box will simply fall to infinity. Using 
    ///controllers can improve the management of a game and game logic.
    ///@example using_controllers.cpp

    //ghost object creation functions

    ///Use this function to create a ghost_box.
    ///
    ///This function creates a ghost_box. A ghost box is a box that can go 
    ///through everything. OSG is used to display the box. If there is a failure 
    ///to create the object then a null pointer is returned.
    ///@param x The x coordinate of the position of the box.
    ///@param y The y coordinate of the position of the box.
    ///@param z The z coordinate of the position of the box.
    ///@param size_x The size of the box on the x axis.
    ///@param size_y The size of the box on the y axis.
    ///@param size_z The size of the box on the z axis.
    ///@param osg_manager The ncc::osg::manager used to create the box.
    ghost_box* create_ghost_box(
            double x,
            double y,
            double z,
            double size_x,
            double size_y,
            double size_z,
            osg::manager& osg_manager);

    ///Use this function to create a ghost_sphere.
    ///
    ///This function creates a ghost_sphere. A ghost sphere is a sphere that can 
    ///go through everything. OSG is used to display the sphere. If there is a 
    ///failure to create the object then a null pointer is returned.
    ///@param x The x coordinate of the initial position.
    ///@param y The y coordinate of the initial position.
    ///@param z The z coordinate of the initial position.
    ///@param radius The radius of the sphere.
    ///@param osg_manager The ncc::osg::manager used to create the sphere.
    ghost_sphere* create_ghost_sphere(
            double x,
            double y,
            double z,
            double radius,
            osg::manager& osg_manager);

    ///Use this function to create a ghost_mesh.
    ///
    ///This function creates a ghost_mesh. A ghost mesh is a mesh that can go 
    ///through everything. OSG is used to load a model from the file specified 
    ///and display the mesh. If the mesh is already cached by ncc::osg::manager 
    ///then the file does not have to be loaded. If there is a failure to create 
    ///the object then a null pointer is returned.
    ///@param file_name The file you want to load. 
    ///@param x The x coordinate of the initial position.
    ///@param y The y coordinate of the initial position.
    ///@param z The z coordinate of the initial position.
    ///@param osg_manager The ncc::osg::manager used to create the sphere.
    ///@return A pointer to the new object on success, and 0 on failure.
    ghost_mesh* create_ghost_mesh(
            const std::string& file_name,
            double x,
            double y,
            double z,
            osg::manager& osg_manager);

    //physical object creation functions

    ///Use this function to create a box.
    ///
    ///This function creates a box that looks and acts like a box. It uses ODE 
    ///to simulate the physics. Creating a box requires a ncc::osg::manager and 
    ///a ncc::ode::manager. If there is a failure to create the object then a 
    ///null pointer is returned.
    ///@param x The x coordinate of the initial position.
    ///@param y The y coordinate of the initial position.
    ///@param z The z coordinate of the initial position.
    ///@param size_x The size of the box on the x axis.
    ///@param size_y The size of the box on the y axis.
    ///@param size_z The size of the box on the z axis.
    ///@param mass The mass of the box.
    ///@param ode_manager A ncc::ode::manager is required.
    ///@param osg_manager A ncc::osg::manager is required.
    ///@return A pointer to the new object on success, and 0 on failure.
    box* create_box(
            double x,
            double y,
            double z,
            double size_x,
            double size_y,
            double size_z,
            double mass,
            ode::manager& ode_manager,
            osg::manager& osg_manager);

    ///Use this function to create a sphere.
    ///
    ///This function creates a sphere that looks and acts like a sphere. It use 
    ///ODE to simulate the physics. Creating the sphere requires a 
    ///ncc::osg::manager and a ncc::ode::manager If there is a failure to create 
    ///the object then a null pointer is returned.
    ///@param x The x coordinate of the initial position.
    ///@param y The y coordinate of the initial position.
    ///@param z The z coordinate of the initial position.
    ///@param radius The radius of the sphere.
    ///@param mass The mass of the cylinder.
    ///@param ode_manager A ncc::ode::manager is required.
    ///@param osg_manager A ncc::osg::manager is required.
    ///@return A new object on success, and 0 on failure.
    sphere* create_sphere(
            double x,
            double y,
            double z,
            double radius,
            double mass,
            ode::manager& ode_manager,
            osg::manager& osg_manager);

    ///Use this function to create a cylinder.
    ///
    ///This function creates a cylinder that looks and acts like a cylinder. It use 
    ///ODE to simulate the physics. Creating the cylinder requires a 
    ///ncc::osg::manager and a ncc::ode::manager If there is a failure to create 
    ///the object then a null pointer is returned.
    ///@param x The x coordinate of the initial position.
    ///@param y The y coordinate of the initial position.
    ///@param z The z coordinate of the initial position.
    ///@param radius The radius of the cylinder.
    ///@param length The length of the cylinder.
    ///@param mass The mass of the cylinder.
    ///@param ode_manager A ncc::ode::manager is required.
    ///@param osg_manager A ncc::osg::manager is required.
    ///@return A new object on success, and 0 on failure.
    cylinder* create_cylinder(
            double x,
            double y,
            double z,
            double radius,
            double length,
            double mass,
            ode::manager& ode_manager,
            osg::manager& osg_manager);


    ///Use this functio to create a mesh.
    ///
    ///This function creates a mesh which is useful for creating static worlds. 
    ///If the mass paramater is 0 then the object is static. If the mass 
    ///parameter is greater than 0 then the object is dynamic and has accurate 
    ///collision detection. However, a dynamic mesh with accurate collision 
    ///detection is currently experimental. The reason is that ODE currently 
    ///does not have good trimesh to trimesh collision. Therefore it is 
    ///recommened to only use this functio to create static meshes by giving a 
    ///mass of 0. If there is a failure to create the object then a null pointer 
    ///is returned.
    ///@param x The x coordinate of the initial position.
    ///@param y The y coordinate of the initial position.
    ///@param z The z coordinate of the initial position.
    ///@param mass The mass of the mesh.
    ///@param ode_manager A ncc::ode::manager is required.
    ///@param osg_manager A ncc::osg::manager is required.
    ///@return A new object on success, and 0 on failure.
    mesh* create_mesh(
            const std::string& file_name,
            double x,
            double y,
            double z,
            double mass,
            ode::manager& ode_manager,
            osg::manager& osg_manager);

    ///Use this function to create a box_bound_mesh.
    ///
    ///A box bound mesh is a mesh which is bound by a rigid body box. Unlike 
    ///ncc::osg_ode::mesh a box bound mesh can be used for dyamic meshes. The 
    ///mesh is essentially surrounded by an invisible box. The box is 
    ///automatically created depending on the mesh. If there is a failure to 
    ///create the object then a null pointer is returned.
    ///@param x The x coordinate of the initial position.
    ///@param y The y coordinate of the initial position.
    ///@param z The z coordinate of the initial position.
    ///@param mass The mass of the mesh.
    ///@param ode_manager A ncc::ode::manager is required.
    ///@param osg_manager A ncc::osg::manager is required.
    ///@return A new object on success, and 0 on failure.
    box_bound_mesh* create_box_bound_mesh(
            const std::string& file_name,
            double x,
            double y,
            double z,
            double mass,
            ode::manager& ode_manager,
            osg::manager& osg_manager);

    ///Use this functio to create a capuse_mesh.
    ///
    ///A capsule mesh is similar to a ncc::osg_ode::box_bound_mesh except that 
    ///it is covered by a capsule shape. A capsule shape is similar to a pill. 
    ///It is a cylinder capped with half spheres. The one property of this 
    ///capsule is that no matter what it will always remain upright. This allows 
    ///a mesh to collide with the enviornment and move around without falling 
    ///over. If there is a failure to create the object then a null pointer is 
    ///returned.
    ///@param x The x coordinate of the initial position.
    ///@param y The y coordinate of the initial position.
    ///@param z The z coordinate of the initial position.
    ///@param mass The mass of the mesh.
    ///@param ode_manager A ncc::ode::manager is required.
    ///@param osg_manager A ncc::osg::manager is required.
    ///@return A new object on success, and 0 on failure.    
    capsule_mesh* create_capsule_mesh(
            const std::string& file_name,
            double x, 
            double y,
            double z,
            double radius,
            double length,
            double mass,
            ode::manager& ode_manager,
            osg::manager& osg_manager);


    invisible_capsule* create_invisible_capsule(double x, 
            double y, 
            double z, 
            double radius,
            double length,
            double mass,
            ode::manager& ode_manager);
    ///@} objects
}//namespace osg_ode
}//namespace ncc
#endif

