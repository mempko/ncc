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

#include "object/osg/osg_policies.h"
namespace ncc {
namespace osg
{
    bool object::load_texture(const std::string& file_name)
    {
       texture_data_cache::data_ptr texture = get_manager().texture_cache().get_data(file_name);

        if(!texture)
        {
            texture = texture_data_cache::data_ptr(new osg_lib::Texture2D());
            texture->setImage (osgDB::readImageFile (file_name));
            
            if(!texture)
            {
                std::cerr << "Could not load texture: " << file_name << std::endl;
                return false;
            }
            get_manager().texture_cache().cache_data(file_name, texture);
        }
        osg_lib::ref_ptr<osg_lib::StateSet> state (new osg_lib::StateSet());
        state->setTextureAttributeAndModes (0, texture.get(), osg_lib::StateAttribute::ON);   // Associate this state set with the Geode that contains
        // the pyramid: 
        position_transform->setStateSet(state.get());
        return true;
    }

    void object::attach_to_parent(osg_lib::ref_ptr<osg_lib::Group> parent_node)
    {
        //attach this object to the parent node
        if(position_transform == 0)
        {
            parent_node_ptr = parent_node;
            position_transform = osg_lib::ref_ptr<osg_lib::PositionAttitudeTransform>(new osg_lib::PositionAttitudeTransform);
        }
        parent_node->addChild (position_transform.get());
    }
        
    void object::update_orientation(double x, double y, double z, double w)
    {
         position_transform->setAttitude (osg_lib::Quat(x, y, z, w));
    }
    
    void object::update_position(double x, double y, double z)
    {
          position_transform->setPosition (osg_lib::Vec3 (x, y, z));
    }

    void box::create_visual_body(double size_x, double size_y, double size_z, manager& mgr)
    {
        attach_to_parent(mgr.root());      
        set_manager(mgr);
        box_ptr = new osg_lib::Box (osg_lib::Vec3 (0.0, 0.0, 0.0), size_x, size_y, size_z); 
        geode_ptr = new osg_lib::Geode;
        geode_ptr->addDrawable (new osg_lib::ShapeDrawable (box_ptr.get())); 
        position_transform->addChild (geode_ptr.get());
    }

    void sphere::create_visual_body(double radius, manager& mgr)
    {
        attach_to_parent(mgr.root());      
        set_manager(mgr);
        sphere_ptr =  new osg_lib::Sphere(osg_lib::Vec3 (0.0, 0.0, 0.0), radius);
        geode_ptr = new osg_lib::Geode;
        geode_ptr->addDrawable (new osg_lib::ShapeDrawable (sphere_ptr.get())); 
        position_transform->addChild (geode_ptr.get());
    }
  
    void cylinder::create_visual_body(double radius, double length, manager& mgr)
    {
        attach_to_parent(mgr.root());      
        set_manager(mgr);
        cylinder_ptr =  new osg_lib::Cylinder(osg_lib::Vec3 (0.0, 0.0, 0.0), radius, length);
        geode_ptr = new osg_lib::Geode;
        geode_ptr->addDrawable (new osg_lib::ShapeDrawable (cylinder_ptr.get())); 
        position_transform->addChild (geode_ptr.get());
    }

    void mesh::create_visual_body(const std::string& file_name, manager& mgr)
    {
        attach_to_parent(mgr.root());      
        set_manager(mgr);            

        mesh_ptr = mgr.node_cache().get_data(file_name);
        
        if(!mesh_ptr)
        {
            mesh_ptr = osgDB::readNodeFile(file_name);
			
            if(!mesh_ptr)
            {
                std::cerr << "Cannot load file: " << file_name;
                return;
            }
            mgr.node_cache().cache_data(file_name, mesh_ptr);
			
			osgUtil::Optimizer optOSGFile;
			optOSGFile.optimize (mesh_ptr.get());
        }
	        position_transform->addChild (mesh_ptr.get());
    }
    
    template <class value_type>
    value_type absolute_value(value_type value)
    {
        return value < 0 ? -value : value;
    }
    void mesh::get_bounding_box(double& size_x, double& size_y, double& size_z, 
                                                double& center_x, double& center_y, double& center_z)
    {
        if(!mesh_ptr) return;
        osg_lib::BoundingBox bb; 
        bb.expandBy(mesh_ptr->getBound());
        size_x = absolute_value(bb.xMax())+absolute_value(bb.xMin());
        size_y = absolute_value(bb.yMax())+absolute_value(bb.yMin());
        size_z = absolute_value(bb.zMax())+absolute_value(bb.zMin());
        center_x = bb.center().x();
        center_y = bb.center().y();
        center_z = bb.center().z();

    }


    void construct_trimesh_data(osg_lib::Geode* geode, std::vector<double>& vertices, std::vector<int>& indices);

    void mesh::get_trimesh_data(std::vector<double>& vertices,  std::vector<int>& indices) 
    {
            for_each_geode(mesh_ptr.get(), boost::bind(construct_trimesh_data, _1 ,boost::ref(vertices), boost::ref(indices)));
    }

    void construct_trimesh_data(osg_lib::Geode* geode, std::vector<double>& vertices, std::vector<int>& indices)
    {
        osg_lib::Geometry* geometry =dynamic_cast<osg_lib::Geometry*>(geode->getDrawable(0));
         if(!geometry)
        {
            std::cerr << "Cannot get geometry"  << std::endl;
            return;
        }
      
        osg_lib::Vec3Array* vertices_array = dynamic_cast<osg_lib::Vec3Array*>(geometry->getVertexArray());
         if(!vertices_array)
         {
            std::cerr << "Cannot get vertices"  << std::endl;
            return;
        }
        
        int base_index = vertices.size() / 3; //divided by 3 because every 3 elements in the array represent the x y and z corrinates of the vertex.
        //base index is needed because we may be constructing a trimesh recursively out of a tree of geodes
        //since each geode has its own indices which index to a vertex at base 0, we need to adust the base as
        //we get more vertices into the main vertex array.
        
        //get the verticies
        int vertex_count = vertices_array->getNumElements();
        const float * vertices_ptr= (const float *)vertices_array->getDataPointer();
        for(int k = 0, j=0; k < vertex_count; k++, j+=3)
        {
            vertices.push_back(vertices_ptr[j+0]);
            vertices.push_back(vertices_ptr[j+1]);
            vertices.push_back(vertices_ptr[j+2]);
        } 
        
        osg_lib::IndexArray* indices_array = geometry->getVertexIndices();
        if(indices_array)                
        {       //create the indices array
                int index_count=indices_array->getNumElements();
                for(int i=0; i < index_count; i++)
                        indices.push_back(indices_array->index(i)+base_index); //we have to adjust the base index so that the indices point
                                                                                                      //at the correct vertex  
        } 
        else
        {
           int num_prim_set = geometry->getNumPrimitiveSets();
            for(int i=0; i<num_prim_set; i++)
            {
                    osg_lib::PrimitiveSet* prim_set =geometry->getPrimitiveSet(i);
                    switch(prim_set->getMode())
                    {
                            case(osg_lib::PrimitiveSet::TRIANGLES):
                            {
                                    int num_indices = prim_set->getNumIndices();
                                    for(int j=0; j < num_indices; j++){
                                                    indices.push_back(prim_set->index(j)+base_index);
                                    }
                            }
                            break;
                            case(osg_lib::PrimitiveSet::TRIANGLE_STRIP):
                            {
                                    int num_indices = prim_set->getNumIndices();
                                    for(int j=0; j < num_indices-2; j++){
                                            indices.push_back(prim_set->index(j+0)+base_index);
                                            indices.push_back(prim_set->index(j+1)+base_index);
                                            indices.push_back(prim_set->index(j+2)+base_index);
                                    }
                            }
                            break;
                            case(osg_lib::PrimitiveSet::TRIANGLE_FAN):
                            {
                                    int num_indices = prim_set->getNumIndices();
                                    for(int j=0; j < num_indices-2; j+=2){
                                            indices.push_back(prim_set->index(0)+base_index);
                                            indices.push_back(prim_set->index(j+1)+base_index);
                                            indices.push_back(prim_set->index(j+2)+base_index);
                                    }
                            }
                            break;
                            case(osg_lib::PrimitiveSet::QUADS):
                            {
                                    int num_indices = prim_set->getNumIndices()/4;
                                    for(int j=0; j < num_indices; j++){
                                            indices.push_back(prim_set->index(4*j+0)+base_index);
                                            indices.push_back(prim_set->index(4*j+1)+base_index);
                                            indices.push_back(prim_set->index(4*j+2)+base_index);
                                            
                                            indices.push_back(prim_set->index(4*j+0)+base_index);
                                            indices.push_back(prim_set->index(4*j+2)+base_index);
                                            indices.push_back(prim_set->index(4*j+3)+base_index);
                                    }
                            }
                            break;
                            case(osg_lib::PrimitiveSet::QUAD_STRIP):
                            {
                                    int num_indices = prim_set->getNumIndices();
                                    for(int j=0; j < num_indices-2; j+=2){
                                            indices.push_back(prim_set->index(j+0)+base_index);
                                            indices.push_back(prim_set->index(j+1)+base_index);
                                            indices.push_back(prim_set->index(j+3)+base_index);
                                            
                                            indices.push_back(prim_set->index(j+0)+base_index);
                                            indices.push_back(prim_set->index(j+3)+base_index);
                                            indices.push_back(prim_set->index(j+2)+base_index);
                                    }
                            }
                            break;
                            case(osg_lib::PrimitiveSet::POLYGON):
                                    debug_message<DEBUG>("unsupport polygon callback");
                    } 
            }
        }
    }
}//namespace osg
}//namespace ncc
