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


#ifndef NCCENTRIFUGE_MESSAGE_CONTROLLER_H
#define NCCENTRIFUGE_MESSAGE_CONTROLLER_H

#include <list>
#include <vector>
#include <algorithm>
#include <boost/utility.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "controller/message.h"
#include "controller/controller_interface.h"
namespace ncc {
namespace controller {
namespace message
{
    class manager;

    ///Holds message information used by ncc::controller::message::manager.
    ///
    ///This class is exclusively used ncc::controller::message::manager. It 
    ///stores several pieces of information which are important in sending a 
    ///message to the right recipient. One feature is that it holds a callback 
    ///to a method which determines if a controller is a recipient. This is 
    ///important because the existence of recipients can change from one frame 
    ///to another. Note the only way to create a message is by the constructor. 
    ///This class is meant to be used by the library only.
    struct message : boost::noncopyable
    {
        ///Constructs a message. 
        ///@param sender A raw pointer to the sender controller
        ///@param is_recipient_callback This is a callback which determines if a 
        ///controller is a recipient of the message. 
        ///@param msg_parcel This is the actual message which needs to be sent. 
        message(abstract_interface* sender, 
                const boost::function<bool (ptr&)>& is_recipient_callback, 
                const parcel& msg_parcel) : 
            is_recipient(is_recipient_callback),
            from(sender),
            message_parcel(msg_parcel),
            sent(false) {}
        boost::function<bool (ptr)> is_recipient;
        abstract_interface* from;
        parcel message_parcel;
        bool sent;
    };
    typedef boost::shared_ptr<message> ptr;


    ///Used by ncc::controller::manager to manage messages which are sent from 
    ///controller to controller.
    ///
    ///A class which help manage messages for ncc::controller::manager. All the 
    ///controller manager does is add messages to the manager and the manager 
    ///takes care of the rest. There are only two methods to the class, one to 
    ///add a message, and one which sends the messages already in the queue.
    ///controller manager gives the message manager its root node and the 
    ///manager does the rest. 
    ///\n\n
    ///There are several important requirements for the message to be sent.
    ///The sender and the reciever of the message must still exist or the 
    ///message cannot be sent. This is so that the controller::abstract_interface 
    ///handle_message method abstract_interface is met. 
    class manager  : boost::noncopyable
    {
        public:
            ///Adds a message to the message manager.
            ///
            ///@param msg A boost::shared_ptr of a new 
            ///ncc::controller::message::message object.
            void add_message(ptr& msg);
            ///sends all messages to the proper recipients in the controller tree 
            ///specified by the root parameter.
            ///
            ///This method transverses the controller tree from the root node in a
            ///depth first type fashion. For each controller it comes across it 
            ///goes through all messages and determines if the controller is a 
            ///recipient of any of the messages. If it is the message is passed to 
            ///the controller's handle_message method.
            ///@param root The root of the controller tree the messages will be sent.
            void send_messages(controller::ptr& root);
        private:
            typedef std::list<ptr> message_list;
            message_list messages;
    };

}//namespace message
}//namespace controller
}//namespace ncc
#endif

