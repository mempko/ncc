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
#include "object/object_manager.h"
#include "controller/controller_manager.h"
#include <list>

//In example 4, we learned how to use the object manager to manage many objects at
//once. However, what if we want each object ot have its own logic. NCCentrifuge
//provides the concept of a controller to accomplish this. Using controllers
//is not manditory, however, they provide a simple yet powerful way to add 
//logic to objects in a generic way. This example demonstrates creating 
//controllers and putting some logic in them. We will create a float controller
//which will create a box and make it float up when 'f' key is pressed.

//notice that the float controller inherits from the controller abstract interface.
//The abstact interface defines all the methods which a controller must have.
//As with many other resources in NCCentrifuge, controllers have a corresponding
//manager (ncc::controller::manager)
class float_controller : public ncc::controller::abstract_interface
{
    public:

        //The initialize method is the only method required of a controller.
        //The construction of a controller is split up between the constructor
        //and the initialize method. This is to allow for prototype functionality
        //in the controller manager. See the reference documentation for more 
        //information. The initialize method takes one parameter which is of type
        //ncc::parameter_list. A parameter list is actually an array of 
        //ncc::parameter objects. Parameter objects can store any type of data
        //within them (see boost::any). This allows the initialize method to
        //be able to take an arbitrary amount of parameters of arbitrary types.
        //our particular initialize method will take 3 parameters (The x, y, and z
        //coordinates of where the box should be created).
        bool initialize(const ncc::parameter_list& params)
        {
            //first we check to see if 3 coordinates are passed.
            if(params.size() == 3)
            {
                //We can retrieve values from the parameters through
                //various helper functions. ncc::get is one such function.
                //more are located in (ncc/elements/parameter.h)
                double x = ncc::get<double>(params[0]);
                double y = ncc::get<double>(params[1]);
                double z = ncc::get<double>(params[2]);

                //We will create a rigid body box using the osg_ode part of 
                //the library. 
                ncc::osg_ode::box_ptr box(ncc::osg_ode::create_box(
                            x, y, z,  //position
                            1, 1, 1,  //size
                            1,        //mass
                            odeManager,
                            osgManager));

                //Lets texture the box with some image.
                box->load_texture("Data/Black.png");

                //Lets make the box bouncy so we can see
                //can make the simulation a little more fun
                box->set_bounce(0.3);

                //Add some friction to the box
                box->set_friction(0.6);

                //This will add the box to the object manager.
                //object is defined as a member below.
                object = objectManager.add_object(box);

                //we return true if the controller was initialized successfully.
                //if there was an error we can return false and the controller will
                //not be added.
                return true;		
            }
            return false;
        }

        //The control method is where the majority of the logic
        //will be put. Here we will check to see if the 'f' key is pressed
        //and float the box upward if that is the case.
        bool control()
        {
            if(osgManager.key_pressed('f'))
            {
                //since object is a boost::weak_ptr, we
                //must lock it to retrieve the coorisponding
                //boost::shared_ptr object. This is very useful for
                //having safe code by making sure the object is not
                //destroyed during the session. This makes object management
                //more or less thread safe.
                if(ncc::object::ptr obj = object.lock())
                    obj->add_force(0.0, 0.0, 2.0);
            }
            return true;
        }

        //Since the float controller requires a couple of managers to
        //create the box, we will store them as references and have the
        //user of the float_controller pass in each manager. 
        //You are free to pass in whatever you like. The controller portion
        //of NCCentrifuge is completely independent from the rest of the library.
        float_controller(ncc::osg::manager& osgMgr,
                ncc::ode::manager& odeMgr,
                ncc::object::manager& objectMgr) 
            : osgManager(osgMgr),
            odeManager(odeMgr),
            objectManager(objectMgr) {}

        //It is important to have a constructor for this controller
        //because it manages a resource. 
        //we want to make sure when the controller is destroyed,
        //the object is created is destroyed also.
        ~float_controller()
        {
            //since we used the ncc::object::manager to manage the object,
            //we can destroy the object by removing it. This is also why
            //its important that we used ncc::object::weak_ptr as the type
            //of our object member variable. This ensures the reference count
            //to our object is 1 and therefore object management is more
            //predictable.
            objectManager.remove_object(object);
        }
    private:
        ncc::osg::manager& osgManager;
        ncc::ode::manager& odeManager;
        ncc::object::manager& objectManager;
        ncc::object::weak_ptr object;
};



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

    //Lets create an object manager to manage the ground and box.
    ncc::object::manager objectManager;

    //We also need to create a controller manager to manage our bounce_controllers
    ncc::controller::manager controllerManager;

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

    //Lets add the ground to the object manager.
    objectManager.add_object(ground);

    //We then initialize the window.
    //This is the point the window is actually created.
    osgManager.initialize();

    //This will store whether the a key is down or up.
    //we want to only add one box when the 'a' key is pressed.
    bool keyDown = false;


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
        objectManager.step();

        //We now update all of our controllers.
        controllerManager.step();

        //Lets test if the 'a' key is pressed
        //if it is we will create a new float controller
        //which will control a box
        if(osgManager.key_pressed('a'))
        {
            if(!keyDown)
            {	
                //Lets create a float_controller. We must pass in all
                //the relevent managers which we use in the controller.
                ncc::controller::ptr new_controller(new float_controller(osgManager, 
                            odeManager,
                            objectManager));
                //lets now define some variables to store the x, y, and z
                //position of where we want our box to be added														  
                double x = 0.0;
                double y = 0.0;
                double z = 10.0;
                //we call now add the controller to the controller manager passing
                //in the x, y, and z as parameters. ncc::parameters function will
                //create a ncc::parameter_list which is passed to our controllers 
                //initialze function. ncc::parameters function can take up to
                //15 arbitrary parameters. Meaning the types passed to the
                //ncc::parameters function can be any type you want!
                //However, since the float_controller needs 3 doubles to create
                //the box, we must pass in the correct parameters. Keeping track
                //of which parameters which controllers accept is up to the
                //developer.
                controllerManager.add_controller(new_controller, ncc::parameters(x, y, z));
            }
            keyDown = true;
        }
        else 
            keyDown = false;

    }
    return 0;
}
