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

#ifndef NCCENTRIFUGE_ODE_POLICIES_H
#define NCCENTRIFUGE_ODE_POLICIES_H

#include <list>
#include <vector>
#include <algorithm>
#include <ode/ode.h>
#include "object/ode/ode_manager.h"
#include "utilities/matrix_3d.h"
#include "utilities/quaternion.h"

namespace ncc {
namespace ode
{
    ///Material struct to hold physical material values
    ///
    ///The default constructor values were created empirically.
    ///This class is used by ncc::ode::object as a holder of
    ///specific physical parameters of a rigid body object
    struct object_material
    {
        double mass;
        double bounce;
        double friction;
        object_material() : mass(1.0), bounce(0.0), friction(0.0) {} 
    };

    ///@addtogroup physics
    ///@{
    ///@addtogroup physics_policy_classes
    ///@{

    ///An ODE wrapper of a rigid body
    ///
    ///Ode uses rigid bodies for motion and ncc::ode::object is a wrapper of 
    ///that functionality. A rigid body is something with a position, velocity, 
    ///and angular velocity obeying the laws of physics. This class is an 
    ///abstract base class and therefore cannot be used directly. There is no 
    ///point in using a rigid body without some kind of shape to define the body. 
    ///@see ncc::ode::box and ncc::ode::sphere for examples of classes which 
    ///derive from ncc::ode::object. 
    ///\n
    ///An important design aspect to note is that classes derived from 
    ///ncc::ode::object can be used in the physical_body template parameter of 
    ///ncc::object. 
    class object : boost::noncopyable
    {
        public:
            object();
            virtual void set_mass(double mass) = 0; 
            virtual void add_force(double x, double y, double z) { if(body_id) {dBodyAddForce (body_id, x , y, z); dBodyEnable(body_id);}}		
            virtual void add_torque(double x, double y, double z) {if(body_id) { dBodyAddTorque(body_id, x, y, z); dBodyEnable(body_id);}}
            virtual void add_relative_force(double x, double y, double z) { if(body_id) {dBodyAddRelForce (body_id, x , y, z); dBodyEnable(body_id);}}
            virtual void add_relative_torque(double x, double y, double z) { if(body_id) {dBodyAddRelTorque(body_id, x, y, z); dBodyEnable(body_id);}}
            virtual void get_position(double& x, double& y, double& z) const;
            virtual void set_position(double x, double y, double z);
            virtual void get_orientation(double& x, double& y, double& z, double& w) const;
            virtual void set_orientation(double x, double y, double z, double w);

            virtual void get_velocity(double& x, double& y, double& z) const;
            virtual void set_velocity(double x, double y, double z);

            virtual double get_mass() const {return material.mass;}
            virtual void set_bounce(double bounce) { material.bounce = bounce;}
            virtual double get_bounce() const { return material.bounce;}
            virtual void set_friction(double friction) {material.friction = friction;}
            virtual double get_friction() const { return material.friction;}

            virtual void update(){}


            //extra functions
            virtual void add_force_at_pos(double x, double y, double z, 
                    double px, double py, double pz) 
            { if(body_id) dBodyAddForceAtPos(body_id, x, y, z, px, py, pz);}

            virtual void add_force_at_relative_pos(double x, double y, double z, 
                    double px, double py, double pz) 
            { if(body_id) dBodyAddForceAtRelPos(body_id, x, y, z, px, py, pz);}

            virtual void add_relative_force_at_pos(double x, double y, double z, 
                    double px, double py, double pz) 
            { if(body_id) dBodyAddRelForceAtPos(body_id, x, y, z, px, py, pz);}

            virtual void add_relative_force_at_relative_pos(double x, double y, double z, 
                    double px, double py, double pz) 
            { if(body_id) dBodyAddRelForceAtRelPos(body_id, x, y, z, px, py, pz);}


            void set_collision_callback(collision_callback callback){ collision_callback_ptr = callback;}
            collision_callback& callback(){ return collision_callback_ptr;}
            ///Returns the dBodyID associated with the rigid body.
            ///
            ///The dBodyID can be used to do more advanced things with the ODE api. 
            dBodyID get_ode_body() { return body_id;}		
            virtual ~object() {if(body_id) dBodyDestroy(body_id);}
        protected:
            ///Create a rigid body at a certain position
            ///
            ///This method creates a rigid body at the location specified by the 
            ///x, y, and z paramaters. This method is used internally by derived 
            ///objects to create a rigid body
            virtual void create_rigid_body(double x, double y, double z, manager& mgr);

            void set_geom_data(dGeomID geom);
            dWorldID world_id;
            dSpaceID space_id;
            dBodyID body_id;
            object_material material;
            collision_callback collision_callback_ptr;
            ///finish collision callback code
    };

    class collidable_object : public object
    {
        public:
            collidable_object() : geom_id(0) , object() {}
            virtual void get_orientation(double& x, double& y, double& z, double& w) const;
            virtual void get_position(double& x, double& y, double& z) const;
            virtual void set_position(double x, double y, double z);
            virtual void set_orientation(double x, double y, double z, double w);
            virtual ~collidable_object() { if(geom_id) dGeomDestroy(geom_id);}
        protected:
            dGeomID geom_id;
    };

    ///A rigid body box class.
    ///
    ///This class creates a rigid body box. This box can have arbitrary width, 
    ///height, and length sizes. Use the create_physical_body method to create 
    ///the box.
    class box: public collidable_object
    {
        public:
            box() : collidable_object() {size[0] = size[1] = size[2] = 0;}

            ///Create the box
            ///
            ///Creates a box which can be used in an ODE physics simulation.
            ///Creates a box with a specified position, size, and mass. 
            ///@param x The x coordinate of the position
            ///@param y The y coordinate of the position
            ///@param z The z coordinate of the position
            ///@param size_x The size of the box on the x axis
            ///@param size_y The size of the box on the y axis
            ///@param size_z The size of the box on the z axis
            ///@param mgr The ode::manager you would like to manage the box simulation 
            ///including the collision detection and rigid body physics.
            void create_physical_body(
                    double x, 
                    double y, 
                    double z, 
                    double size_x, 
                    double size_y, 
                    double size_z, 
                    double mass, 
                    manager& mgr);
            virtual void set_mass(double mass);
        protected:
            double size[3];
    };

    ///A rigid body sphere class.
    ///
    ///This class creates a rigid body sphere. This sphere can have arbitrary 
    ///size. To create the sphere call create_physical_body.
    class sphere: public collidable_object
    {
        public:
            sphere() : collidable_object() {}

            ///Creates the rigid body sphere
            ///
            ///This creates a sphere which can be used in a ode physics simulation. 
            ///Creates a sphere with a specific position, radius, and mass.
            ///@param x The x coordinate of the position
            ///@param y The y coordinate of the position
            ///@param z The z coordinate of the position      
            ///@param mgr The ode::manager you would like to manage the sphere 
            ///simulation including collision detection and rigid body physics.
            void create_physical_body(
                    double x, 
                    double y, 
                    double z, 
                    double radius, 
                    double mass, 
                    manager& mgr);
            virtual void set_mass(double mass);
        protected:
            double radius;
    };

    ///A rigid body cylinder class.
    ///
    ///This class creates a rigid body cylinder. This cylinder can have arbitrary 
    ///radius and length. To create the cylinder call create_physical_body.
    class cylinder: public collidable_object
    {
        public:
            cylinder() : collidable_object() {}

            ///Creates the rigid body sphere
            ///
            ///This creates a sphere which can be used in a ode physics simulation. 
            ///Creates a sphere with a specific position, radius, and mass.
            ///@param x The x coordinate of the position
            ///@param y The y coordinate of the position
            ///@param z The z coordinate of the position      
            ///@param mgr The ode::manager you would like to manage the sphere 
            ///simulation including collision detection and rigid body physics.
            void create_physical_body(
                    double x, 
                    double y, 
                    double z, 
                    double radius, 
                    double length, 
                    double mass, 
                    manager& mgr);
            virtual void set_mass(double mass);
        protected:
            double radius;
            double length;
    };


    trimesh_data* create_trimesh_data(std::vector<double>& vertices_vec, std::vector<long>& indices_vec);

    ///A rigid body mesh
    ///
    ///This class can be used to represent an arbitrary mesh. Because of the 
    ///limitations ODE currently has, moving meshes do not work very well when 
    ///they collide with other moving meshes. However, static meshes work great 
    ///and can be used to represent an arbitrary static game world where other 
    ///rigid bodies collide against.
    class trimesh : public collidable_object
    {
        public:
            trimesh() : mesh_data() , collidable_object(){size[0] = size[1] = size[2] = 0;}

            ///Creates the trimesh rigid body.
            ///
            ///Use this to create the trimesh rigid body. A trimesh is a triangle 
            ///mesh, meaning all polygons are trianglees. This is a restriction of 
            ///ODE. The paramaters vertices_vec and indices_vec give the trimesh the
            ///data it needs. vertices_vec holds the vertices of the trimesh and and 
            ///indices_vec holds the indices of each polygon.
            ///\n\n
            /// Note The format for vertices_vec is an array of doubles where every 
            ///3 doubles represents one vertex. so vertices_vec[0] is the x, 
            ///vertices_vec[1] is the y, and vertices_vec[2] is the z. 
            ///vertices_vec[3] would be x for the next vertices etc... indices_vec 
            ///holds a list of integers where every 3 represents a triangle. 
            ///\n\n
            ///The beauty is that ncc::osg::mesh has functions to grab the 
            ///vertices_vec and indices_vec in the correct format from a file.
            ///@see ncc::osg_ode::create_mesh function an example of how easy it is 
            ///to create an ode trimesh from an osg mesh. 
            ///\n\n
            ///Also note that if the trimesh data already exists in the cache then 
            ///it is not computed again. Also note that the size_x, size_y, and 
            ///size_z parameters are used to create a box mass tensor.
            ///@param size_x Is the x size of the bounding box of the mesh. 
            ///@param size_y Is the y size of the bounding box of the mesh. 
            ///@param size_z Is the z size of the bounding box of the mesh. 
            ///@param name If a string of size > 0 is passed, then this trimesh is 
            ///cached for future use by ode::manager. So if we were to create an 
            ///object of the same trimesh, it would just be grabbed from the cache! 
            ///This speeds things up quite a bit while saving memory significantly. 
            void create_physical_body(
                    double x, 
                    double y, 
                    double z, 
                    double size_x, 
                    double size_y,
                    double size_z,
                    double mass, 
                    std::vector<double>& vertices_vec,
                    std::vector<int>& indices_vec,
                    const std::string name,
                    manager& mgr);        
            virtual void set_mass(double mass);
            virtual void update();
        protected:
            trimesh_data_cache::data_ptr mesh_data;
            double size[3];
    };

    ///Creates a rigid body capsule.
    ///
    ///A rigid body capsule is important for enclosing meshes that you want 
    ///psydo physics for. The capsule is a capped cylinder shape. Think of a 
    ///pill you would chew down when you have a head ache. One particular 
    ///property of this capsule is that no matter what it stays up right even 
    ///when it knocks into things. This is useful for many things. For example, 
    ///lets say you want a first person view like in a shooter game (FPS). You 
    ///would want your character to walk around and slide on walls but you never 
    ///want them to fall over as a real pill shape object would. The reason a 
    ///pill is good is that it allows for turning without knocking into things
    ///and walking up objects like stairs and such because the bottom is a half 
    ///sphere.
    class capsule : public collidable_object
    {
        public:
            capsule() : radius(0), length(0), collidable_object(){}

            ///Creates the rigid body capsule
            ///
            ///A rigid body capsule which stands upright. 
            ///@param radius The radius of the half spheres at the bottom and top of 
            ///the capsule.
            ///@param length The length of the cylinder part of the capsule.
            void create_physical_body(
                    double x,
                    double y,
                    double z,
                    double radius,
                    double length,
                    double mass,
                    manager& mgr);
            virtual void set_mass(double mass);          
        private:
            dJointID amotor_id;
            double radius;
            double length;
    };
    ///@} physics_policy_classes
    ///@} physics

}//namespace ode
}//namespace ncc
#endif

