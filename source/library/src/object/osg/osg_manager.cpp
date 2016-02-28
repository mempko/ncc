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

#include "object/osg/osg_manager.h"

namespace ncc {
namespace osg 
{
    manager::manager(int x, int y, int width, int height, bool full_screen)
    {
        //create the open scene graph part
        root_node = osg_lib::ref_ptr<osg_lib::Group>(new osg_lib::Group);
		if(!full_screen)
			viewer.setUpViewInWindow(x, y, width, height);
        viewer.setSceneData (root_node.get());
        previous_time = osg_lib::Timer::instance()->tick();
        key_event_handler = osg_lib::ref_ptr<key_handler>(new key_handler);
        add_handler(key_event_handler.get()); 
        realized = false;
	}
	
    void manager::add_handler(osgGA::GUIEventHandler* event_handle)
    {
        if(event_handle) viewer.addEventHandler(event_handle);
    }
	
    void manager::initialize()
    {
        if(!realized) 
        {
            viewer.realize(); 
            realized = true;
        }
    }
	
    void manager::step()
    {
        viewer.frame();
    }
	
	void manager::get_camera_position(double& x, double& y, double& z) const
	{
			const osg_lib::Vec3d  osg_camera_pos = viewer.getCamera()->getViewMatrix().getTrans();
			x = osg_camera_pos.x();
			y = osg_camera_pos.y();
			z = osg_camera_pos.z();					
	}
	void manager::get_camera_orientation(double& x, double& y, double& z, double& w) const
	{
		osg_lib::Quat osg_camera_orientation = viewer.getCamera()->getViewMatrix().getRotate();
		x = osg_camera_orientation.x(); 
		y = osg_camera_orientation.y();
		z = osg_camera_orientation.z();
		w = osg_camera_orientation.w();
	}
	void manager::look_at(double cam_x, double cam_y, double cam_z, 
					 double obj_x, double obj_y, double obj_z, 
					 double up_x, double up_y, double up_z)
	{
       osg_lib::Vec3d eye(cam_x, cam_y, cam_z);
       osg_lib::Vec3d center(obj_x, obj_y, obj_z);
       osg_lib::Vec3d up(up_x, up_y, up_z);
	   viewer.getCamera()->setViewMatrixAsLookAt(eye, center, up);
									   
	}
	
	void manager::set_mouse_position(float x, float y)
	{
		//KeyboardMouse* km = viewer.getKeyboardMouse();
		//km->positionPointer(x,y);
	}

}//namespace osg
}//namespace ncc
