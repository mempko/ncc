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
#include "utilities/vector_3d.h"
#include "controller/controller_manager.h"
#include "object/object_manager.h"
#include "scripting/script_utilities.h"

//This application runs a ncc game script specified in the command line.

using namespace boost;
using namespace std;

// - main ----------------------------------------------------------------------
int main (int argc, char** argv)
{
    if(argc != 2) 
    {
        std::cout << "Usage: " << argv[0] << " <lua script>" << std::endl;
        return 0;
    }

    std::string scriptFile = argv[1];
    std::cout << "Running: " << scriptFile << std::endl;

    //Setup all the appropriate resource managers

    //create the OpenDynamicsEngine physics manager
    ncc::ode::manager odeManager;
    odeManager.set_gravity(0.0, 0.0, -9.8);

    //create the OpenSceneGraph visual manager
    ncc::osg::manager osgManager(50, 50, 800, 600);

    //create the OpenAL sound manager
    ncc::oal::manager oalManager;

    ncc::object::manager objectManager;
    ncc::property::manager propertyManager;
    ncc::controller::manager controllerManager;

    //Load the script specified at the command line and
    //create a controller from it
    ncc::lua::controller::ptr mainController(
            new ncc::lua::controller(scriptFile, 
                ncc::lua::utilities::bind_ncc, 
                osgManager, 
                odeManager, 
                oalManager,
                objectManager, 
                controllerManager,
                propertyManager));
    controllerManager.add_controller(mainController, ncc::parameter_list());

    osgManager.initialize();
    while (!osgManager.done())
    {

        //Step the view
        osgManager.step();

        //step the physics simulation 
        odeManager.step(0.01);
        //update all controllers
        controllerManager.step();
        //update all objects
        objectManager.step();

    }
}



