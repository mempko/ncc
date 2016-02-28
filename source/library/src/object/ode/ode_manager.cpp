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

#include "object/ode/ode_manager.h"
#include "object/ode/ode_policies.h"
namespace ncc {
namespace ode
{
    manager::manager(double erp, double cfm) : ERP(erp), CFM(cfm)
    {
        dInitODE();
        world_id = dWorldCreate();
        dWorldSetCFM (world_id,CFM);
        dWorldSetERP(world_id,ERP);
        dWorldSetGravity (world_id,0,0,-1.0);//-9.8);

        // dWorldSetQuickStepNumIterations(World,80);
        dWorldSetAutoDisableFlag (world_id,1);
        dWorldSetAutoDisableLinearThreshold(world_id,0.08);
        dWorldSetAutoDisableAngularThreshold(world_id,0.08);
        dWorldSetContactMaxCorrectingVel (world_id,3.0);
        dWorldSetContactSurfaceLayer (world_id,0.1);

        //Create a collision world and collision geometry objects, as necessary.
        dVector3 center = {0,0,0};
        dVector3 extends = {500, 500, 500};
        space_id = dQuadTreeSpaceCreate(0,center,extends,10);
     //   space_id = dSimpleSpaceCreate(0);
        //Create a joint group to hold the contact joints.
       contact_group_id = dJointGroupCreate(0);

    }
    
    void near_callback (void* mgr, dGeomID o1, dGeomID o2)
    {
        //get the manager pointer
        manager* ode_manager= reinterpret_cast<manager*>(mgr);

        const int MAX_CONTACTS = 64; // maximum number of contact points per body
        int i;
        // exit without doing anything if the two bodies are connected by a joint
        dBodyID b1 = dGeomGetBody(o1);
        dBodyID b2 = dGeomGetBody(o2);
        if (b1 && b2 && dAreConnectedExcluding (b1,b2,dJointTypeContact)) return;

        //now get a pointer to the objects stored in the geom data pointer
        object* object_1 =  reinterpret_cast<object*>(dGeomGetData(o1));
        object* object_2 =  reinterpret_cast<object*>(dGeomGetData(o2));
        
        //if we could not get the objects then return
        if(!object_1 || !object_2)
            return;
    
        const double o1_friction = object_1->get_friction();
        const double o2_friction = object_2->get_friction();
        const double friction = std::sqrt(o1_friction * o2_friction);
        const double bounce = (object_2->get_bounce() + object_1->get_bounce()) / 2.0;
    
        dContact contact[MAX_CONTACTS];   
        dContact object_contact;
        
        object_contact.surface.mode = dContactBounce | dContactSoftCFM;
        object_contact.surface.mu = friction == -1 ? dInfinity : friction;
        object_contact.surface.mu2 = object_contact.surface.mu;
        object_contact.surface.bounce = bounce;
        object_contact.surface.bounce_vel = 0.01;
        object_contact.surface.soft_cfm = 0.001;

        for (i=0; i<MAX_CONTACTS; i++) 
            contact[i] = object_contact;
        
        if (int numc = dCollide (o1,o2,MAX_CONTACTS,&contact[0].geom,sizeof(dContact))) 
        {
			collision_callback& callback1 = object_1->callback();
			collision_callback& callback2 = object_2->callback();
			collision_info collision1 = { 0,dynamic_cast<ncc::object::abstract_interface*>(object_1), dynamic_cast<ncc::object::abstract_interface*>(object_2)};
			collision_info collision2 = { 0, dynamic_cast<ncc::object::abstract_interface*>(object_2), dynamic_cast<ncc::object::abstract_interface*>(object_1)};
			bool create_joints;
			if(callback1  && callback2)
			{
				bool col1 = callback1(collision1);
				bool col2 = callback2(collision2);
				create_joints = col1 || col2;
			}
			else if(callback1 && !callback2)
				create_joints = callback1(collision1);
			else if(callback2 && !callback1)
				create_joints = callback2(collision2);
			else
				create_joints = true;
			
			if(create_joints)
				for (i=0; i<numc; i++) 
				{
					dJointID c = dJointCreateContact (ode_manager->ode_world(),ode_manager->contact_group(),&contact[i]);
					dJointAttach (c,b1,b2);
				}
        }
    }


	struct ray_contact_holder
	{
		double contact_depth;
		ode::object* contact_object;
	};

	void near_ray_callback(void* ray_contact_ptr,  dGeomID o1, dGeomID o2)
	{
		dContact contact;
		
		if(ray_contact_holder* ray_contact= reinterpret_cast<ray_contact_holder*>(ray_contact_ptr))
		{	
			
			if( dCollide( o2, o1, 1, &contact.geom, sizeof(contact) ) == 1 ) 
			{			
				if( contact.geom.depth < ray_contact->contact_depth) 
				{					
					ray_contact->contact_depth = contact.geom.depth;
					ray_contact->contact_object = reinterpret_cast<ode::object*>(dGeomGetData(o2));					 
				}
			}
		}
	}
	double manager::ray_cast(double origin_x, double origin_y, double origin_z, 
							double direction_x, double direction_y, double direction_z, 
							double length, ode::object** obj)
	{		
		ray_contact_holder ray_contact = { 1e10, 0};		
		dGeomID ray = dCreateRay(0, length);		
		dGeomRaySet(ray, origin_x, origin_y, origin_z, direction_x, direction_y, direction_z );
		dSpaceCollide2( ray, reinterpret_cast<dGeomID>(space_id), reinterpret_cast<void*>(&ray_contact), near_ray_callback );		
		(*obj) = ray_contact.contact_object;			
		dGeomDestroy(ray);
		return ray_contact.contact_depth;
	}

    void manager::step(double step_size)
    {
        dSpaceCollide (space_id, reinterpret_cast<void*>(this), near_callback);  //do collision detection on the space
        dWorldQuickStep (world_id, step_size);      //step the simulation
        dJointGroupEmpty (contact_group_id);      //empty all the collision contacts
    }
    manager::~manager()
    {
        dSpaceDestroy(space_id);
        dWorldDestroy(world_id);
    }


}//namespace ode
}//namespace ncc
