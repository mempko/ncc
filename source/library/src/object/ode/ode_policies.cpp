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

#include "object/ode/ode_policies.h"
namespace ncc {
namespace ode
{
    trimesh_data* create_trimesh_data(std::vector<double>& vertices_vec, std::vector<int>& indices_vec)
    {
        trimesh_data* new_trimesh = new trimesh_data();
        
        //construct the verticies
        new_trimesh->vertex_count = vertices_vec.size() / 3;
        if(!new_trimesh->vertex_count)
        {
            delete new_trimesh;
            return 0;
        }
              
        new_trimesh->vertices = boost::shared_array<dVector3>(new dVector3[new_trimesh->vertex_count]);
        int j;
        for(int k = 0, j = 0; k < new_trimesh->vertex_count; k++, j +=3)
        {
            new_trimesh->vertices[k][0] = vertices_vec[j + 0];
            new_trimesh->vertices[k][1] = vertices_vec[j + 1];
            new_trimesh->vertices[k][2] = vertices_vec[j + 2];
        }            
        
        int index_count=indices_vec.size();
        if(!index_count)
        {
            delete new_trimesh;
            return 0;
        }
     
        new_trimesh->indices = boost::shared_array<dTriIndex>(new dTriIndex[index_count]);
        std::copy(indices_vec.begin(), indices_vec.end(), new_trimesh->indices.get());
        
          // Create TriMesh in ODE
        new_trimesh->data_id = dGeomTriMeshDataCreate();
        dGeomTriMeshDataBuildSimple(
                new_trimesh->data_id, 
                (dReal*)new_trimesh->vertices.get(), 
                new_trimesh->vertex_count,
                (const dTriIndex*)new_trimesh->indices.get(), 
                index_count);
       // dGeomTriMeshDataPreprocess(new_trimesh->data_id);
                                                        
        return new_trimesh;
        
    }
    
    object::object() : world_id(0), space_id(0), body_id(0), material()
    {
    //....currently empty
    }
    void object::get_orientation(double& x, double& y, double& z, double& w) const
    {
		if(!body_id) return;
        const dReal* quat = dBodyGetQuaternion (body_id);
        x = quat[1]; y = quat[2]; z = quat[3]; w = quat[0];
    } 
    void object::get_position(double& x, double& y, double& z) const
    {
		if(!body_id) return;
        const dReal* vec = dBodyGetPosition (body_id);
        x = vec[0]; y = vec[1]; z = vec[2];
    }
    void object::set_position(double x, double y, double z)
    {
        if(body_id) dBodySetPosition (body_id,x, y, z);
    }
    
    void object::set_orientation(double x, double y, double z, double w)
    {
		if(!body_id) return;
        dQuaternion quat = { w, x, y, z};
        dBodySetQuaternion(body_id,quat);
    }
	
	void object::set_velocity(double x, double y, double z)
	{
		if(body_id) dBodySetLinearVel(body_id,x,y,z);
	}
	void object::get_velocity(double& x, double& y, double& z) const
	{
		if(!body_id) return;
		const dReal* vec = dBodyGetLinearVel(body_id);
		x = vec[0]; y = vec[1]; z = vec[2];
	}
	
    void object::create_rigid_body(double x, double y, double z, manager& mgr)
    {
		body_id = dBodyCreate (world_id);
        dBodySetPosition (body_id,x, y, z);
        dBodySetAutoDisableDefaults(body_id);
    }
    
    void object::set_geom_data(dGeomID geom)
    {
        if(geom)
        {
            //for the materials to work correctly when two objects collide we must
            //store a pointer to the class itself in the geom. This alows us to cast back
            //in the collision near callback function used by ode_manager and get the
            //bouncyness, friction, etc, of the object.
            dGeomSetData(geom,reinterpret_cast<void*>(this));
        }
    }
	
	void collidable_object::get_orientation(double& x, double& y, double& z, double& w) const
    {
		if(body_id) 
		{
			object::get_orientation(x, y, z, w);
			return;
		}
		
		if(!geom_id) return;
        dQuaternion quat; dGeomGetQuaternion (geom_id, quat);
        x = quat[1]; y = quat[2]; z = quat[3]; w = quat[0];
    } 
    void collidable_object::get_position(double& x, double& y, double& z) const
    {
		if(body_id) 
		{
			object::get_position(x, y, z);
			return;
		}
		
		if(!geom_id) return;
        const dReal* vec = dGeomGetPosition (geom_id);
        x = vec[0]; y = vec[1]; z = vec[2];
    }
    void collidable_object::set_position(double x, double y, double z)
    {
		if(body_id)
		{
			object::set_position(x, y, z);
			return;
		}
		
		if(!geom_id) return;
        dGeomSetPosition (geom_id,x, y, z);
    }
    void collidable_object::set_orientation(double x, double y, double z, double w)
    {
		if(body_id)
		{
			object::set_orientation(x, y, z, w);
			return;
		}
		if(!geom_id) return;
        dQuaternion quat = { w, x, y, z};
        dGeomSetQuaternion(geom_id,quat);
    }


    void box::create_physical_body(
                    double x, 
                    double y, 
                    double z, 
                    double size_x, 
                    double size_y, 
                    double size_z, 
                    double mass, 
                    manager& mgr)
    {
         world_id = mgr.ode_world();
         space_id = mgr.ode_space();

        //create and position the geom to represent the pysical shape of the rigid body   
        geom_id = dCreateBox (mgr.ode_space(), size_x, size_y, size_z);
        object::set_geom_data(geom_id);
        dGeomSetPosition (geom_id, x, y, z); 		
		size[0] = size_x;
        size[1] = size_y;
        size[2] = size_z;

		if(mass > 0)
		{
			object::create_rigid_body(x, y, z, mgr);
			set_mass(mass);
			dGeomSetBody (geom_id, body_id);  
		}
    }

    void box::set_mass(double mass)
    {
        if(body_id)
        {
            dMass dmass;
            dMassSetZero(&dmass); 
            dMassSetBoxTotal (&dmass, mass, size[0], size[1], size[2]);
            dBodySetMass (body_id, &dmass);
            material.mass = mass;
        }
    }

    void sphere::create_physical_body(
                    double x, 
                    double y, 
                    double z, 
                    double radius, 
                    double mass, 
                    manager& mgr)
    {
		this->radius = radius;

		world_id = mgr.ode_world();
		space_id = mgr.ode_space();
        //create and position the geom to represent the physical shape of the rigid body   
        geom_id = dCreateSphere(mgr.ode_space(),radius);
        object::set_geom_data(geom_id);
        dGeomSetPosition (geom_id, x, y, z); 		
		
		if(mass > 0)
		{
			object::create_rigid_body(x, y, z, mgr);
			dGeomSetBody (geom_id, body_id);  
			set_mass(mass);
		}

    }

    void sphere::set_mass(double mass)
    {
        if(body_id)
        {
            dMass dmass;
            dMassSetZero(&dmass); 
            dMassSetSphereTotal(&dmass,mass,radius);
            dBodySetMass (body_id, &dmass);
            material.mass = mass;
        }
    }

    void cylinder::create_physical_body(
                    double x, 
                    double y, 
                    double z, 
                    double radius, 
                    double length, 
                    double mass, 
                    manager& mgr)
    {
		this->radius = radius;
		this->length = length;

		world_id = mgr.ode_world();
		space_id = mgr.ode_space();
        //create and position the geom to represent the physical shape of the rigid body   
        geom_id = dCreateCylinder(mgr.ode_space(),radius, length);
        object::set_geom_data(geom_id);
        dGeomSetPosition (geom_id, x, y, z); 		
		
		if(mass > 0)
		{
			object::create_rigid_body(x, y, z, mgr);
			dGeomSetBody (geom_id, body_id);  
			set_mass(mass);
		}

    }

    void cylinder::set_mass(double mass)
    {
        if(body_id)
        {
            dMass dmass;
            dMassSetZero(&dmass); 
            dMassSetCylinderTotal(&dmass,mass,1,radius,length);
            dBodySetMass (body_id, &dmass);
            material.mass = mass;
        }
    }

    void capsule::create_physical_body(
                        double x,
                        double y,
                        double z,
                        double radius,
                        double length,
                        double mass,
                        manager& mgr)
    {
        this->radius = radius;
        this->length = length;      
	
		world_id = mgr.ode_world();
		space_id = mgr.ode_space();
        //set the body orientation
      //  dMatrix3 R;
        //dRFromAxisAndAngle(R,1,0,0,M_PI/2);
        //dBodySetRotation(body_id,R);

        //create the geom
        geom_id=dCreateCapsule(mgr.ode_space(),radius,length);
        object::set_geom_data(geom_id); //must make sure to set the geom data for the collision callback!
		dGeomSetPosition (geom_id, x, y, z); 		

		if(mass > 0)
		{
			object::create_rigid_body(x, y, z, mgr);
			set_mass(mass);
			dGeomSetBody(geom_id,body_id);
		
			              
			//create an amotor to keep the body vertical
			amotor_id=dJointCreateAMotor(mgr.ode_world(),0);
			dJointAttach(amotor_id,body_id,0);
			dJointSetAMotorMode(amotor_id,dAMotorEuler);
			dJointSetAMotorNumAxes(amotor_id,3); 
			dJointSetAMotorAxis(amotor_id,0,0,1,0,0); 
			dJointSetAMotorAxis(amotor_id,1,0,0,1,0);
			dJointSetAMotorAxis(amotor_id,2,0,0,0,1);
			dJointSetAMotorAngle(amotor_id,0,0);
			dJointSetAMotorAngle(amotor_id,1,0);
			dJointSetAMotorAngle(amotor_id,2,0);
			dJointSetAMotorParam(amotor_id,dParamLoStop,-0);
			dJointSetAMotorParam(amotor_id,dParamLoStop3,-0);
			dJointSetAMotorParam(amotor_id,dParamLoStop2,-0);
			dJointSetAMotorParam(amotor_id,dParamHiStop,0);
			dJointSetAMotorParam(amotor_id,dParamHiStop3,0);
			dJointSetAMotorParam(amotor_id,dParamHiStop2,0);
		
		}
    }
    void capsule::set_mass(double mass)
    {
      if(body_id)
        {
            dMass dmass;
            dMassSetZero(&dmass); 
            dMassSetSphereTotal(&dmass,mass,radius);
            dMassSetCapsuleTotal(&dmass,mass,3,radius,length);
            dBodySetMass (body_id, &dmass);
            material.mass = mass;
        }
    }

    void trimesh::create_physical_body(
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
                   manager& mgr)
    {
        world_id = mgr.ode_world();
        space_id = mgr.ode_space();

        //see if the trimesh data is cached so that we don't have to recreate it
        if(name.size() > 0) mesh_data = mgr.trimesh_cache().get_data(name);

        //if it is not cached then we create the trimesh data and put it in the cache
        if(!mesh_data)
        {
            trimesh_data* new_data = create_trimesh_data(vertices_vec, indices_vec);
            if(new_data == 0)
            {
                std::cout << "Error creating trimesh: " << name << std::endl;
                return; //TODO should throw instead...
            }
            mesh_data = trimesh_data_cache::data_ptr(new_data);
            mgr.trimesh_cache().cache_data(name, mesh_data);
        }

        //create the geom using the trimesh data
        geom_id = dCreateTriMesh(mgr.ode_space(),mesh_data->data_id, 0, 0, 0); 

        object::set_geom_data(geom_id);
        //create and position the geom to represent the pysical shape of the rigid body   
        dGeomSetPosition (geom_id, x, y, z); //we position it at the center relative to the body
        //if the mass is greater than 0 then the object is a dynamic mesh
        //so we have to create a rigid body
        if(mass > 0)
        {
            create_rigid_body(x, y, z,  mgr);
            size[0] = size_x;
            size[1] = size_y;
            size[2] = size_z;
            set_mass(mass);
            dGeomSetBody (geom_id, body_id); 
        }
    }

    void trimesh::set_mass(double mass)
    {
        if(body_id)
        {   
            dMass dmass;
            dMassSetBoxTotal (&dmass, mass, size[0], size[1], size[2]);
            dBodySetMass (body_id, &dmass);
            material.mass = mass;
        }
    }

    void trimesh::update() 
    {
        double qx, qy, qz, qw, x, y, z;
        get_orientation(qx, qy, qz, qw);
        get_position(x, y, z);
        quaterniond quat(qx, qy, qz, qw);
        matrix_3dd matrix;
        quat.create_matrix(matrix);
        matrix.translate(x, y, z);
        
        dReal* matrix_data = (dReal*)matrix.raw_matrix();
        dGeomTriMeshSetLastTransform(geom_id, matrix_data); 
    }

}//namespace ode
}//namespace ncc
