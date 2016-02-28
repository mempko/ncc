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

#ifndef NCCENTRIFUGE_OBJECT_POLICIES_H
#define NCCENTRIFUGE_OBJECT_POLICIES_H
#include <boost/bind.hpp>
#include <string>
#include "utilities/unicode.h"
#include "object/osg/osg_manager.h"
namespace osg_lib = osg;
namespace ncc {
namespace osg
{
    ///@addtogroup opengl
    ///@{
    ///@addtogroup visual_policy_classes
    ///@{

    ///A wrapper to an OSG object
    ///
    ///This is the base class to all OSG policy classes which can be used as the 
    ///visual_body template parameter in ncc::object<>. OSG simplifies displaying 
    ///OpenGL objects in many ways. OSG has many useful utilities for rapidly 
    ///developing an OpenGL application. ncc::osg::object further simplifies the 
    ///use of OSG.
    class object : boost::noncopyable
    {
        public:
            typedef osg_lib::ref_ptr< osg_lib::PositionAttitudeTransform> position_transform_type;
            typedef osg_lib::ref_ptr< osg_lib::Group> group_type;

            ///Returns the position transform of the object
            ///
            ///The position transform is sort of the 'root' of the object. It is 
            ///more or holds the world position of the object. Changing the position 
            ///transform changes the position and orientation of the object.
            position_transform_type get_position_transform(){return position_transform;}

            ///Attached a texture to the object.
            ///
            ///This method loads an image from a file and textures the object with 
            ///it. If the image already exists in the cache then the file does not 
            ///have to be read. 
            ///@return True on success.
            bool load_texture(const std::string& file_name); 

            ///Changes the orientation of the position transform.
            virtual void update_orientation(double x, double y, double z, double w);

            ///Changes the position of the position transform.
            virtual void update_position(double x, double y, double z);      

            object() : parent_node_ptr(0),position_transform(0), manager_ptr(0){}
            virtual ~object() {parent_node_ptr->removeChild(position_transform.get());}
        protected:
            ///Attaches the position_transform to the parent_node.
            virtual void attach_to_parent(osg_lib::ref_ptr<osg_lib::Group> parent_node);

            ///Sets the manager which the object will use.
            void set_manager(manager& mgr) { manager_ptr = &mgr;}

            ///Returns a refrence to the manager.
            manager& get_manager() { return *manager_ptr;}

            ///Updates the object. 
            ///
            ///Any custom update code goes here. An example code can be custom 
            ///animation code.
            virtual void update(){}

        protected:
            group_type parent_node_ptr;
            position_transform_type position_transform;
            manager* manager_ptr;
    };

    ///An OSG box.
    ///
    ///This class is an OSG box with specified size. If a simple box shape is 
    ///needed in a game then this is a quick solution. Use load_texture to add a 
    ///texture to the box. 
    class box : public object
    {
        public:
            box() : geode_ptr(0), box_ptr(0), object() {}

            ///Creates the box with a specific size.
            ///
            ///Creates a simple box of the specified size. A ncc::osg::manager is 
            ///required to create the box.
            ///@param size_x The size of the box on the x axis.
            ///@param size_y The size of the box on the y axis.
            ///@param size_z The size of the box on the z axis.
            ///@param mgr The ncc::osg::manager to use to create the box.
            void create_visual_body(double size_x, double size_y, double size_z, manager& mgr);		
        protected:
            osg_lib::ref_ptr<osg_lib::Geode> geode_ptr;
            osg_lib::ref_ptr<osg_lib::Box> box_ptr;
    };

    ///An OSG sphere.
    ///
    ///This class is an OSG sphere. If a game requires a sphere shape then this 
    ///is a simple solution. The radius of the sphere can be specified. Use the 
    ///load_texture method to give the sphere a texture.
    class sphere : public object
    {
        public:
            sphere() : geode_ptr(0), sphere_ptr(0), object(){}
            ///Creates the sphere with a specific radius.
            ///
            ///A ncc::osg::manager is required to create the box.
            ///@param radius The radius of the sphere.
            ///@param mgr An ncc::osg::manager to use to create the box.
            void create_visual_body(double radius, manager& mgr);
        protected:
            osg_lib::ref_ptr<osg_lib::Geode> geode_ptr;
            osg_lib::ref_ptr<osg_lib::Sphere> sphere_ptr;
    };

    ///An OSG cylinder.
    ///
    ///This class is an OSG cylinder. If a game requires a cylinder shape then this 
    ///is a simple solution. The radius and length of the cylinder can be specified. Use the 
    ///load_texture method to give the cylinder a texture.
    class cylinder : public object
    {
        public:
            cylinder() : geode_ptr(0), cylinder_ptr(0), object(){}
            ///Creates the sphere with a specific radius.
            ///
            ///A ncc::osg::manager is required to create the box.
            ///@param radius The radius of the sphere.
            ///@param mgr An ncc::osg::manager to use to create the box.
            void create_visual_body(double radius, double length, manager& mgr);
        protected:
            osg_lib::ref_ptr<osg_lib::Geode> geode_ptr;
            osg_lib::ref_ptr<osg_lib::Cylinder> cylinder_ptr;
    };

    ///An OSG mesh.
    ///
    ///A mesh is a polygonal object. This class is extremely useful for loading 
    ///mesh models from files. A model editor such as Blender 
    ///(http://www.blender.org) can be used to create 3d models and this class 
    ///can be used to display them. Any model format that OSG supports can be 
    ///used. Use load_texture to texture the model. Note that to texture the 
    ///model, it must have UV coordinates. Another feature of this class is that 
    ///it can be used in combination with ncc::ode::trimesh to create static 
    ///world enviornments. 
    ///\n\n
    ///The way ncc::osg::mesh works with ncc::ode::trimesh is simple. Use 
    ///get_trimesh_data method to get the trimesh data from the OSG mesh. Then 
    ///use that data in the ncc::ode::trimesh create_physical_body method. If 
    ///the mass is 0 then the object is static. However, if the mass is not 0 
    ///then the trimesh is a rigid body. Note however that dynamic trimeshes are 
    ///experimental.
    class mesh : public object
    {
        public:
            mesh() : mesh_ptr(0) , object() {}

            ///Creates the mesh from a file.
            ///
            ///This creates the mesh from a file of any type that is supported by 
            ///OSG. If the mesh already exists in the model cache that
            ///ncc::osg::manager has then the file is not read. 
            ///@param file_name The file which holds the model data.
            ///@param mgr A ncc::osg::manager to use to create the mesh.
            void create_visual_body(const std::string& file_name, manager& mgr);

            ///Generates the trimesh data needed to create an ncc::ode::trimesh.
            void get_trimesh_data(std::vector<double>& vertices,  std::vector<int>& indices);

            ///Returns the bounding radius of the mesh.
            double get_bounding_radius() const {return mesh_ptr->getBound().radius();}

            ///Returns the bounding box of the mesh.
            void get_bounding_box(double& size_x, double& size_y, double& size_z,
                    double& center_x, double& center_y, double& center_z);
        protected:
            node_data_cache::data_ptr mesh_ptr;
    };

    template<class function_type>
        void for_each_geode(osg_lib::Node* current_node, function_type action)
        {
            if ( !current_node)
                return;
            if (osg_lib::Geode* found_geode = dynamic_cast<osg_lib::Geode*>(current_node))
                action(found_geode);
            //if null then current node is not a group
            if (osg_lib::Group* current_group = current_node->asGroup()) 
            {
                const unsigned int num_children = current_group->getNumChildren();
                for (unsigned int i = 0 ; i < num_children; i ++)
                    for_each_geode(current_group->getChild(i), action);
            }
        }
    ///@} visual_policy_classes
    ///@} opengl
}//namespace osg
}//namespace nccentriifuge
#endif

