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
#include <list>

//Lets try out some physics! This example will create a ground, and then drop a box
//from the air onto the ground.
int main(int argc, const char** argv)
{
    //We first create an OpenSceneGraph manager. The manager manages
    //the 3d view and the internal scene graph. We pass the starting position of 
    //the window, the size, and wether or not it is full screen or not.
    ncc::osg::manager osgManager(50, 50, 640, 480, false);

    //To be able to have physics, we will use the Open Dynamics Engine manager. 
    //Open Dynamics Engine is an open source physics engine and the manager is a
    //thin wrapper around its functionality.
    ncc::ode::manager odeManager;

    //We will create the ground. If the mass is 0, then the object is considered
    //to be static and will not be subject to the laws of physics.
    ncc::osg_ode::box_ptr ground(ncc::osg_ode::create_box(
                0, 0, 0,    //position
                10, 10, 1, //size
                0,           //mass
                odeManager,  
                osgManager));
    //Lets texture the ground with an image.
    ground->load_texture("Data/Brown.png");

    //Lets add some friction to the ground.
    ground->set_friction(1.0);    

    ncc::osg_ode::box_ptr box(ncc::osg_ode::create_box(
                0, 0, 10,  //position
                1, 1, 1,  //size
                1,        //mass
                odeManager,
                osgManager));

    //Lets texture the box with some image.
    box->load_texture("Data/Black.png");

    //Lets make the box bouncy so we can see the
    //physics engine work.
    box->set_bounce(0.5);

    //Lets make the box have some friction so we can see the
    box->set_friction(0.6);

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


        //Lets update the physics simulation.
        //This will update the box each step and make it fall.
        odeManager.step(0.05);

        //We update the ground and box position and display.
        ground->update();
        box->update();
    }
    return 0;
}
