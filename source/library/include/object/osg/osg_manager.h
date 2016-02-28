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

#ifndef NCCENTRIFUGE_OSG_MANAGER_H
#define NCCENTRIFUGE_OSG_MANAGER_H
#ifndef WIN32
	#define __USE_OSX_CGL_IMPLEMENTATION__ 1
#endif
#include <osg/Geometry>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/PositionAttitudeTransform>
#include <osg/Texture2D>
#include <osg/Timer>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgGA/GUIEventHandler>
#include <algorithm>
#include <boost/utility.hpp>
#include <boost/progress.hpp>
#include <osgUtil/Optimizer>

#include "utilities/cache.h"
#include "utilities/debug.h"
namespace osg_lib = osg;
namespace ncc {
namespace osg 
{
    class key_handler: public osgGA::GUIEventHandler
    {
        public:
            /// Constructs an \c ODEController.
            key_handler()
            { 
                std::fill_n(key_array, 256, false);
                std::fill_n(mouse_buttons, 4, false);
                mouse_x_pos = mouse_y_pos = 0;
            }
            /// Handle an event.
            virtual bool handle (const osgGA::GUIEventAdapter& ea,
                    osgGA::GUIActionAdapter& aa)
            {
                switch (ea.getEventType())
                {
                    case osgGA::GUIEventAdapter::NONE:
                        break;
                    case osgGA::GUIEventAdapter::KEYDOWN:
                        key_array[ea.getKey()] = true;
                        break;
                    case osgGA::GUIEventAdapter::KEYUP:
                        key_array[ea.getKey()] = false;
                        break;
                    case osgGA::GUIEventAdapter::PUSH:
                        mouse_buttons[translate_mouse_button(ea.getButton())] = true;
                        break;
                    case osgGA::GUIEventAdapter::RELEASE:
                        mouse_buttons[translate_mouse_button(ea.getButton())] = false;
                        break;
                    case osgGA::GUIEventAdapter::MOVE:
                        mouse_x_pos = ea.getX();//normalized();
                        mouse_y_pos = ea.getY();//normalized();
                        break;
                        /*		case osgGA::GUIEventAdapter::DRAG:
                                mouse_buttons[translate_mouse_button(ea.getButton())] = true;
                                mouse_x_pos = ea.getX();//normalized();
                                mouse_y_pos = ea.getY();//normalized();
                                return true;*/
                    default:
                        return false;
                }
                return true;
            }

            int translate_mouse_button (int button)
            {
                switch (button)
                {
                    case osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON:
                        return 0;
                    case osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON:
                        return 1;
                    case osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON:
                        return 2;
                    default:
                        return 3;
                }
            }	


            bool key(char c) { return key_array[c];}
            float mouse_x() { return mouse_x_pos;}
            float mouse_y() { return mouse_y_pos;}
            bool button(int b) { return b < 3 ? mouse_buttons[b] : mouse_buttons[3];} 
        private:

            bool key_array[256]; 
            bool mouse_buttons[4];
            float mouse_x_pos, mouse_y_pos;
    };


    typedef cache<osg_lib::Texture2D, std::string, osg_lib::ref_ptr> texture_data_cache;
    typedef cache<osg_lib::Node, std::string, osg_lib::ref_ptr> node_data_cache;

    ///Manages the OSG scene graph.
    ///
    ///All objects that use OSG need this manager to be created. This manager 
    ///has several useful methods. For one it updates the frame on every call to 
    ///step. It also manages the opengl view using osgViewer. It also caches
    ///things such as models loaded from files and also textures. 
    ///@ingroup managers
    ///@ingroup opengl
    class manager : boost::noncopyable
    {
        public:
            ///This is the only constructor and it initializes the window.
            ///
            ///@param x The x cordinate of the window position in pixels.
            ///@param y The y cordinate of the window position in pixels.
            ///@param width The width of the window in pixels
            ///@param height The height of the window in pixels.
            manager(int x, int y, int width, int height, bool full_screen = false);

            ///Adds an OSG even handler to the even handler list.
            ///
            ///The manager starts out with one even handler to handle the keyboard. 
            ///Other custom OSG event handlers can be added with this method.
            void add_handler(osgGA::GUIEventHandler* event_handle);

            ///Returns the root node of the scene graph
            ///
            ///Note that OSG uses it's own refrence counted pointer.
            osg_lib::ref_ptr<osg_lib::Group> root() { return root_node;}

            ///Updates the frame.
            ///
            ///This updates the frame. It draws the scene graph.
            void step();

            ///Initializes the manager.
            ///
            ///This method must be called before the main loop. It actually creates 
            ///("realizes") the window. If this is not called then no window will be 
            ///created.
            void initialize(); 

            ///Returns true if the program is done.
            ///
            ///This is used by the main loop to determine whether it should be 
            ///exited or not.
            bool done(){ return viewer.done();}

            ///Returns true if the char specified is pressed on the keyboard.
            bool key_pressed(char c) { return key_event_handler->key(c);}
            float mouse_x() { return key_event_handler->mouse_x();}
            float mouse_y() { return key_event_handler->mouse_y();}
            void set_mouse_position(float x, float y);
            bool button_pressed(int b) { return key_event_handler->button(b);}

            void get_camera_position(double& x, double& y, double& z) const;
            void get_camera_orientation(double& x, double& y, double& z, double& w) const;
            void look_at(double cam_x, double cam_y, double cam_z, 
                    double obj_x, double obj_y, double obj_z,
                    double up_x = 0.0, double up_y = 0.0, double up_z = 1.0);

            ///Returns the texture cache used by OSG objects.
            ///@{
            texture_data_cache& texture_cache() {return texture2d_cache;}
            const texture_data_cache& texture_cache() const {return texture2d_cache;}
            ///@}

            ///Returns the node cache used by OSG objects
            ///@{
            node_data_cache& node_cache() { return filenode_cache;}
            const node_data_cache& node_cache() const {return filenode_cache;}
            ///@}

        private:
            manager(const manager& other);
            // The scene graph root.
            osg_lib::ref_ptr<osg_lib::Group> root_node;
            osgViewer::Viewer viewer;
            osg_lib::Timer_t previous_time;
            osg_lib::ref_ptr<key_handler> key_event_handler;
            texture_data_cache texture2d_cache;
            node_data_cache filenode_cache;
            bool realized;
    };
}//namespace osg
}//namespace ncc
#endif

