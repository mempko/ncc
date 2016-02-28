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
#include "sound/oal_manager.h"
#include "utilities/vector_3d.h"
#include "utilities/quaternion.h"
#include <list>

//This example will create a small mini game where you walk around in a
//world which a bunch of random balls chase you down. You can kill the balls
//by pressing the space bar which will shoot a box. This example
//uses controllers, has sound, and other advanced functionality.


//Lets create a structure to easily hold all references to all the managers
//we need to create this game.
struct ManagerHolder
{
	ncc::osg::manager& osgManager;
	ncc::ode::manager& odeManager;
	ncc::oal::manager& oalManager;
	ncc::object::manager& objectManager;
	ncc::controller::manager& controllerManager;
};

//This will store the player position at any time
//so that the enemies can know where the player is.
ncc::vector_3dd player_position;

//We will create 4 controllers to make this game work.
//The first controller is called world_controller and it will be responsible
//for controlling the world. The second controller is the player_controller and
//it will be responsible for controlling the player. The third controller is the
//bullet_controller and it is responsible for the logic of the bullets that
//the player shoots. The fourth controller is the enemy_controller and it 
//controls the spheres in the world.

//To create a controller we inherit from ncc::controller::abstract_interface.
//any child of this class can be managed by a ncc::controller::manager.
//The controller manager updates each controller, passes messages, and
//is able to construct new controllers from prototypes. The enemy controller
//we are creating below will handle messages and also function as a prototype
//so that we can create new enemies on the fly.
class EnemyController : public ncc::controller::abstract_interface
{
    public:
        //Each controller must have an initialization function taking in
        //one parameter of type ncc::parameter_list. A parameter list is a vector
        //of type ncc::parameter.
        bool initialize(const ncc::parameter_list& params)
        {
            //It is the job of the creator of the controller to take
            //the parameters and extract the values.
            //An enemy will take three parameters. 
            //1. position of the enemy.
            //2. raidus of the enemy body.
            //3. mass of enemy.
            if(params.size() == 3)
            {	
                //NCCentrifuge provides a vector class for convenience.
                //To retrieve a value from the parameter, you
                //can use the "get" function.
                ncc::vector_3dd position = ncc::get<ncc::vector_3dd>(params[0]);
                double radius = ncc::get<double>(params[1]);
                double mass = ncc::get<double>(params[2]);

                ncc::osg_ode::sphere_ptr new_sphere(
                        ncc::osg_ode::create_sphere(
                            position.x(), position.y(), position.z(),
                            radius, mass,
                            managers.odeManager,
                            managers.osgManager));
                new_sphere->load_texture("Data/Green.png");
                new_sphere->set_bounce(0.5);
                new_sphere->set_friction(0.8);
                new_sphere->set_id(2);

                //To find out when the enemy collides with its enviornment
                //we can set a collision callback. This is a function that will be called
                //when the enemy collides.
                new_sphere->set_collision_callback(std::bind1st(std::mem_fun(&EnemyController::collision_callback), this));
                enemy_object = managers.objectManager.add_object(new_sphere);
                return true;
            }
            return false;
        }

        //Our collision callback. This will be called whenever the enemy object collides.
        bool collision_callback(const ncc::ode::collision_info info)
        {
            //a bullet has an id of 1 so we 
            //destroy the enemy when the bullet collides with it.
            if(info.object_2->get_id() == 1)
                remove_self();
            return true;
        }

        //This function is called everytime the controller managers's step function is called.
        //we put most of our logic for the enemy in this function.
        bool control()
        {
            if(ncc::object::ptr enemy = enemy_object.lock())
            {
                //we find the direction the player is from the 
                //enemy and move the enemy towards the player.
                ncc::vector_3dd enemy_position;
                enemy->get_position(enemy_position.x(), enemy_position.y(), enemy_position.z());

                ncc::vector_3dd direction = player_position - enemy_position;
                direction.normalize();
                ncc::vector_3dd force = direction * 9.0;
                enemy->add_force(force.x(), force.y(), force.z());
            }
            return true;
        }

        //This is an optional method which handles any messages passed to the controller.
        //Messages are sent using the controller manager. In our case
        //the world controller we will create below will send a destroy message to all
        //controllers when the 'x' key is pressed.
        void handle_message(const ncc::parameter& message, const ncc::parameter_list& params, ncc::controller::abstract_interface& sender)
        {
            //if a destroy message is sent, we will destroy the controller and therefore
            //any objects associated with it.
            if(ncc::get<std::string>(message) == "destroy")
                remove_self();
        }

        //The constructor will take a refrence to the ManagerHolder
        //so that all of the manager functionality is available to
        //the controller.
        EnemyController(ManagerHolder& mgrs) : managers(mgrs) {}
        ~EnemyController()
        {	
            //we will play the explode sound which, by now, was already registered
            //in the world controller.
            managers.oalManager.play("explode", 1.0, false);
            //we must remove our object from the object manager
            //so that it dissapears instead of existing without any
            //logic.
            managers.objectManager.remove_object(enemy_object);
        }
    private:
        //This is an optional method available to controllers.
        //This method should return a new controller from the heap
        //as a clone of the current object in the state before
        //the initialize method is called. Usually this means
        //just calling the constructor.
        ncc::controller::abstract_interface* clone_prototype()
        {
            return new EnemyController(managers);
        }
    private:
        ncc::object::weak_ptr enemy_object;
        ManagerHolder& managers;
};

class BulletController : public ncc::controller::abstract_interface
{
    public:
        bool initialize(const ncc::parameter_list& params)
        {
            if(params.size() == 2)
            {
                ncc::vector_3dd position = ncc::get<ncc::vector_3dd>(params[0]);
                ncc::vector_3dd velocity = ncc::get<ncc::vector_3dd>(params[1]);
                ncc::vector_3dd size(1.0, 1.0, 1.0);
                double mass = 1.0;

                ncc::osg_ode::box_ptr box(
                        ncc::osg_ode::create_box(
                            position.x(), position.y(), position.z(),
                            size.x(), size.y(), size.z(),
                            mass,
                            managers.odeManager,
                            managers.osgManager));
             
                box->load_texture("Data/Black.png");
                box->set_id(1);
                box->set_bounce(0.6);
                box->set_friction(1.0);
                box->set_velocity(velocity.x(), velocity.y(), velocity.z());
                box->set_collision_callback(std::bind1st(std::mem_fun(&BulletController::collision_callback), this));

                bullet = managers.objectManager.add_object(box);
                return true;
            }
            return false;
        }
        void handle_message(const ncc::parameter& message, const ncc::parameter_list& params, ncc::controller::abstract_interface& sender)
        {
            if(ncc::get<std::string>(message) == "destroy")
                remove_self();
        }

        bool collision_callback(const ncc::ode::collision_info info)
        {
            //an enemy has an id of 2 so we
            //will destroy our bullet when it hits
            //the enemy.
            if(info.object_2->get_id() == 2)
                remove_self();
            return true;
        }

        BulletController(ManagerHolder& mgrs) : managers(mgrs) {}
        ~BulletController()
        {
            managers.objectManager.remove_object(bullet);
        }
    private:
        ncc::controller::abstract_interface* clone_prototype()
        {
            return new BulletController(managers);
        }
    private:
        ncc::object::weak_ptr bullet;
        ManagerHolder& managers;

};

class PlayerController : public ncc::controller::abstract_interface
{
    public:
        bool initialize(const ncc::parameter_list& params)
        {
            if(params.size() == 1)
            {
                ncc::vector_3dd position = ncc::get<ncc::vector_3dd>(params[0]);
                ncc::osg_ode::invisible_capsule_ptr capsule(
                        ncc::osg_ode::create_invisible_capsule(
                            position.x(), position.y(), position.z(),
                            2, 2, 1,
                            managers.odeManager));

                capsule->set_friction(0.9);
                capsule->set_mass(3.0);
                player = managers.objectManager.add_object(capsule);
                direction.set(0.0, -1, 0.0);
                keyPressed = false;
                return true;
            }
            return false;
        }

        bool control()
        {
            //Whenever we want to retrieve and use our object
            //we must lock our weak_ptr of that object which will
            //return a shared_ptr.
            if(ncc::object::ptr player = this->player.lock())
            {
                //This part defines logic to control
                //the player when the keyboard is used.
                double yaw = 0.0;
                ncc::vector_3dd position;
                player->get_position(position.x(), position.y(), position.z());

                //make the player turn left and right
                if(managers.osgManager.key_pressed('j'))
                    yaw = 0.01;
                if(managers.osgManager.key_pressed('l'))
                    yaw = -0.01;

                //adjust the direction of the player
                //to rotate depending on
                //the yaw.
                ncc::quaterniond orientation;
                orientation.create_from_euler(0, 0 , yaw);

                direction = orientation * direction;
                direction.normalize();
                ncc::vector_3dd forward_force = direction * 30.0;

                //move the player forwared or backward or make the player float.
                if(managers.osgManager.key_pressed('i'))
                    player->add_force(forward_force.x(), forward_force.y(), forward_force.z());
                if(managers.osgManager.key_pressed('k'))
                    player->add_force(-forward_force.x(), -forward_force.y(), -forward_force.z());
                if(managers.osgManager.key_pressed('y'))
                    player->add_force(0.0, 0.0, 20.0);

                //change the camera to look at the direction
                //the player is pointing and be positioned at the
                //center of the player.	
                ncc::vector_3dd looking_at = position + direction;
                managers.osgManager.look_at(
                        position.x(), position.y(), position.z(), 
                        looking_at.x(), looking_at.y(), looking_at.z());

                player_position = position;

                //lets shoot a bullet whenever 
                //the space bar is pressed.
                if(managers.osgManager.key_pressed(' '))
                {
                    if(!keyPressed) ShootBullet();
                    keyPressed = true;
                }
                else
                    keyPressed = false;
            }	
            return true;
        }
        //This function will create a new bullet controller.
        //in other words it shoots a bullet (which is box shaped.)
        void ShootBullet()
        {
            //play the "fire" sound. This sound is registered by
            //the world controller. 
            managers.oalManager.play("fire", 1.0, false);

            //lets shoot the bullet in the direction the player is facing
            //with a force of 30.
            ncc::vector_3dd velocity = direction * 30.0;
            managers.controllerManager.add_controller("bullet", ncc::parameters(player_position, velocity));
        }
        PlayerController(ManagerHolder& mgrs) : managers(mgrs) {}
        ~PlayerController()
        {
            managers.objectManager.remove_object(player);
        }
    private:
        ncc::object::weak_ptr player;
        ncc::vector_3dd direction;
        bool keyPressed;
        ManagerHolder& managers;
};

class WorldController : public ncc::controller::abstract_interface
{
    public:
        bool initialize(const ncc::parameter_list& params)
        {
            //Lets register the sounds we want to use in this application.
            managers.oalManager.register_sound("Data/explode.wav","explode");
            managers.oalManager.register_sound("Data/fire.wav", "fire");

            //lets create the floor 
            ncc::osg_ode::box_ptr box(
                    ncc::osg_ode::create_box(
                        0, 0, -8, //position
                        100, 100, 5, //size
                        0, //mass
                        managers.odeManager,
                        managers.osgManager));
            box->load_texture("Data/Brown.png");
            box->set_friction(1.0);
            box->set_bounce(0.3);
            floor = managers.objectManager.add_object(box);

            //We can load the world from a file. This particular world
            //was created with blender ("www.blender.org").
            ncc::osg_ode::mesh_ptr mesh(
                    ncc::osg_ode::create_mesh(
                        "Data/level.obj",
                        0, 0, 0, //position
                        0, //mass
                        managers.odeManager,
                        managers.osgManager));

            mesh->load_texture("Data/max.jpg");
            world = managers.objectManager.add_object(mesh);

            //Since enemy controller and bullet controller can be prototypes
            //we can register them with the controller manager and therefore
            //making new enemies or bullets becomes easier.
            ncc::controller::ptr enemyPrototype(new EnemyController(managers));
            ncc::controller::ptr bulletPrototype(new BulletController(managers));

            managers.controllerManager.add_prototype("enemy", enemyPrototype);
            managers.controllerManager.add_prototype("bullet", bulletPrototype);

            //We create and add the player controller.
            ncc::controller::ptr player(new PlayerController(managers));
            ncc::vector_3dd playerPos(0.0, 20.0, 0.0);
            managers.controllerManager.add_controller(player, ncc::parameters(playerPos));

            CreateEnemies();
            keyPressed = false;
            return true;
        }

        bool control()
        {
            //If the s key is pressed we will create new enemies.
            if(managers.osgManager.key_pressed('s'))
            {
                if(!keyPressed) CreateEnemies();
                keyPressed = true;
            }
            //Lets send a destroy message to all controllers
            //when the x key is pressed.
            if(managers.osgManager.key_pressed('x'))
            {
                if(!keyPressed)
                    managers.controllerManager.send_message_to_all(this, std::string("destroy"));
                keyPressed = true;
            }
            else
                keyPressed = false;
        }

        //This function is responsible for creating enemies in the world
        void CreateEnemies()
        {
            int enemies = 20;
            double x = 19.0;
            double y = 10.0;
            double z = 2.0;

            while(enemies--)
            {
                x+= 2.0;
                y+= 2.0;
                ncc::vector_3dd position(x, y, z);
                double radius = 1.0;
                double mass = 1.0;
                //because we registered the enemy controller in the prototype list
                //we can easily create a new controller by name.
                managers.controllerManager.add_controller("enemy", ncc::parameters(position, radius, mass));
            }
        }

        WorldController(ManagerHolder& mgrs) : managers(mgrs){}
        ~WorldController()
        {
            managers.objectManager.remove_object(world);
            managers.objectManager.remove_object(floor);
        }
    private:
        ncc::object::weak_ptr world;
        ncc::object::weak_ptr floor;
        bool keyPressed;
        ManagerHolder& managers;
};

int main(int argc, const char** argv)
{
    ncc::osg::manager osgManager(50, 50, 640, 480, false);
    ncc::ode::manager odeManager;
    ncc::object::manager objectManager;
    ncc::controller::manager controllerManager;
    ncc::oal::manager oalManager;

    //Lets store all our managers in the manager holder for convenience.
    ManagerHolder managers = {
        osgManager, 
        odeManager, 
        oalManager, 
        objectManager, 
        controllerManager};

    odeManager.set_gravity(0.0, 0.0, -9.8);
    ncc::controller::ptr worldController(new WorldController(managers));
    controllerManager.add_controller(worldController, ncc::parameter_list());

    osgManager.initialize();

    while(!osgManager.done())
    {
        osgManager.step();
        odeManager.step(0.01);
        objectManager.step();

        //This will update all the controllers that are managed and therefore
        //the logic of the whole program.
        controllerManager.step();
    }
    return 0;
}
