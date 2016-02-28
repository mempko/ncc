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

//This example demonstrates how to create a window of a specific size and 
//display a box.
int main(int argc, const char** argv)
{
    //We first create an OpenSceneGraph manager. The manager manages
    //the 3d view and the internal scene graph. We pass the starting position of 
    //the window, the size, and wether or not it is full screen or not.
    ncc::osg::manager osgManager(50, 50, 640, 480, false);

    //We then create a box at the position <0, 10, 0> and a size of <1,1,1>.
    //Notice that we pass the OpenSceneGraph manager to the object creation
    //function. If you take your left hand and make a gun shape and then stick
    //out your middle finger perpedicular to your index finger, you will have
    //a representation of the x, y, and z axis. Your thumb is the z axis, your
    //index finger is the y axis, and your middle finger is the x axis. The
    //direction your finger is pointing represents the positive direction.
    ncc::object::ptr box(ncc::osg_ode::create_ghost_box(0, 10, 0, 
                1, 1, 1,
                osgManager));
    //Lets add a force to the box. A ghost travels at a constant velocity.	
    box->add_force(0,0.1,0);

    //We then initialize the window.
    //This is the point the window is actually created.
    osgManager.initialize();

    //This is the main application loop.
    //When the window is closed, done becomes true and
    //the loop ends.
    while(!osgManager.done())
    {
        //We update window every iteration.
        osgManager.step();

        //We update the box position and display.
        box->update();
    }
    return 0;
}
