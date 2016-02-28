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

#include "controller/message_manager.h"
namespace ncc {
namespace controller {
namespace message 
{
    void manager::add_message(ptr& msg)
    {
        if(msg) messages.push_back(msg);
    }

    inline void handle_message(controller::ptr& recipient, controller::abstract_interface* sender, const parameter& msg, const parameter_list& params)
    {
        if(recipient && sender) recipient->handle_message(msg, params, *sender);
    }

    void send_message_to(controller::ptr& recipient, ptr* msg)
    {
        using namespace boost;

        //now we call the callback is_recipient to determine if the current controller is
        //a recipient of the message.
        if((*msg)->is_recipient(recipient))
        {
            handle_message(
                    recipient, 
                    (*msg)->from, 
                    (*msg)->message_parcel.message, 
                    (*msg)->message_parcel.parameters);

            (*msg)->sent = true;
        }
    }

    bool send_message(controller::ptr& controller, std::vector<ptr*>& messages)
    {
        std::for_each(messages.begin(), messages.end(), boost::bind(send_message_to, boost::ref(controller), _1));
        return true;
    }

    bool message_sent(ptr& msg)
    {
        if(!msg->from) return true;   //if the sender no longer exists then the message cannot be sent.
        return msg->sent;                  
    }

    void add_ready_message(ptr& msg, std::vector<ptr*>& messages)
    {
        if(msg->message_parcel.send()) messages.push_back(&msg);
    }	

    void manager::send_messages(controller::ptr& root)
    {
        if(messages.empty()) return;

        //filter messages that are not ready to be sent
        std::vector<ptr*> ready_messages;
        ready_messages.reserve(messages.size());
        std::for_each(messages.begin(), messages.end(), boost::bind(add_ready_message, _1, ref(ready_messages)));
        if(ready_messages.empty()) return;

        //transverse tree and send ready messages
        tree::transverse_depth_first(root, boost::bind<bool>(send_message, _1, boost::ref(ready_messages)));

        //erase all messages that were sent
        messages.remove_if(message_sent);
    }
}//namespace message
}//namespace controller
}//namespace ncc
