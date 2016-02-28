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

#ifndef NCCENTRIFUGE_MESSAGE_H
#define NCCENTRIFUGE_MESSAGE_H

#include <boost/shared_ptr.hpp>
#include <boost/timer.hpp>
#include "elements/parameter.h"
namespace ncc {
namespace controller {
namespace message
{

    namespace condition
    {
        ///interface to all message conditions.
        ///
        ///A message condition determines whether a message should be sent.
        ///The interface is one simple method called "send" which returns a 
        ///boolean. If it returns true, then the message should be sent, 
        ///otherwise it is not ready to be sent. 
        ///\n\n
        ///For use with ncc::controller::message::parcel, a condition must be 
        ///created on the heap! There is a useful typedef for creating 
        ///condition pointers (ncc::controller::message::condition::ptr). 
        ///Here is an example of using a custom condition with parcel.
        ///     - controller::message::condition::ptr myCondition(new MyCustomCondition);
        ///     - controller::message::parcel myParcel(std::string("Hello"), myCondition);
        ///
        ///Where MyCustomCondition a condition you made based on the condition 
        ///interface. The second line creates
        ///a parcel which will be sent only when myCondition is met.
        ///@ingroup interfaces
        struct interface
        {
            virtual bool send() const = 0;
        };
        typedef boost::shared_ptr<condition::interface> ptr;    
    }
    ///Holds a message and its parameters.
    ///
    ///A simple class which holds a message as a ncc::parameter and can hold 
    ///parameters as ncc::parameter_list. This is primarily used by the 
    ///ncc::controller::manager to send messages. A parcel can be thought 
    ///of as a letter which is sent to another controller. The message can be a 
    ///simple "hello", or a more complex message with several parameters. The 
    ///flexibility of the parcel is that it can have a specific condition which
    ///has to be met in order to send the message. A good example is 
    ///controller::message::condition::timed. The timed condition specifies the 
    ///number of seconds which need to pass before the message is sent. You can 
    ///even create your own condition by deriving from 
    ///controller::message::condition::interface. There are also several other 
    ///pre-made conditions which are useful as well.
    ///\n\n
    ///@see ncc::controller::message::condition::interface For more about
    ///conditions.
    class parcel
    {
        public:
            ///Creates a simple message.
            ///
            ///Use this constructor to create a simple message. The message is sent 
            ///immediately.  
            parcel(const parameter& msg) : 
                message(msg), 
                parameters(),
                type(immediate),
                condition(){}
            ///Creates a simple message which is sent if a specific condition is met.
            ///
            ///Use this constructor to create a simple message which is sent only if 
            ///a specific condition is met. The condition can be any condition with
            ///the ncc::controller::message::condition::interface.
            ///@see ncc::controller::message::condition::interface
            parcel(const parameter& msg, condition::ptr& cond) : 
                message(msg), 
                parameters(),
                type(conditional),
                condition(cond){}

            ///Creates a message with parameters.
            ///
            ///Use this constructor to create a message with an arbitrary number of 
            ///parameters. Use ncc::parameters function to create the parameters.
            ///@see ncc::parameters
            parcel(const parameter& msg, const parameter_list& params) : 
                message(msg), 
                parameters(params),
                type(immediate),
                condition(){}

            ///Creates a message with parameters which is sent when a condition is 
            ///met.
            ///
            ///Use this constructor to create a message with an arbitrary number of 
            ///parameters. The message is only sent when the condition is met. The 
            ///condition can be any condition with the 
            ///ncc::controller::message::condition::interface. Use ncc::parameters 
            ///function to create the parameters.
            ///@see ncc::controller::message::condition::interface
            ///@see ncc::parameters
            parcel(const parameter& msg, const parameter_list& params, condition::ptr& cond) : 
                message(msg), 
                parameters(params),
                type(conditional),
                condition(cond){}

            ///Copy constructor.    
            parcel(const parcel& other) : 
                message(other.message), 
                parameters(other.parameters),
                type(other.type),
                condition(other.condition){}

            ///Returns true if the message is to be sent.
            bool send() const;

            ///Useful operator to easly chain parameters.
            ///
            ///Use this function to chain parameters. Example:
            ///     - ncc::controller::message::parcel myParcel(std::string("Print"));
            ///     - myParcel("This")("Is")("A")("Message");
            ///
            ///In this example a message "Print" is sent to some controller. Lets 
            ///say whenever the controller the message is sent to receives the 
            ///"Print" message, it prints all the parameters onto the screen with 
            ///spaces in between all parameters. Then the text "This Is A Message" 
            ///should be printed onto the screen. Where each word was a parameter. 
            ///Hopefully this will ease the pain of adding parameters.
            parcel& operator()(const parameter& param) {parameters.push_back(param); return *this;} 

            parameter message; 
            parameter_list parameters;

        private:
            enum {immediate, conditional} type;
            condition::ptr condition;
    };

    namespace condition
    {
        ///A condition which sends a message after a certain amount of second
        ///elapse.
        struct timed : public interface
        {
            timed(double seconds) : send_time(seconds), time() {}
            bool send() const {return time.elapsed() >= send_time;}
            private:
            const double send_time;
            boost::timer time;
        };

        ///A condition which sends a message when a variable is less than a 
        ///specific value.
        template <class type>
            struct less_than_value : public interface
        {
            less_than_value(const type& variable, const type& value) : variable_ref(variable), value_holder(value){}
            bool send() const {return variable_ref < value_holder;}
            private:
            const type& variable_ref;
            type value_holder;
        };

        ///A condition which sends a message when a variable is less than 
        ///another variable. 
        template <class type>
            struct less_than_variable : public interface
        {
            less_than_variable(const type& variable, const type& value) : variable_ref(variable), value_ref(value){}
            bool send() const {return variable_ref < value_ref;}
            private:
            const type& variable_ref;
            const type& value_ref;
        };

        ///A condition which sends a message when a variable is equal to 
        ///another value.
        template <class type>
            struct equal_to_value : public interface
        {
            equal_to_value(const type& variable, const type& value) : variable_ref(variable), value_holder(value){}
            bool send() const {return variable_ref == value_holder;}
            private:
            const type& variable_ref;
            type value_holder;
        };

        ///A condition which sends a message when a variable is equal to another 
        ///variable.
        template <class type>
            struct equal_to_variable : public interface
        {
            equal_to_variable(const type& variable, const type& value) : variable_ref(variable), value_ref(value){}
            bool send() const {return variable_ref == value_ref;}
            private:
            const type& variable_ref;
            const type& value_ref;
        };

        ///A condition which negates another condition.
        template<class condition_type>
            struct is_not : public interface
        {
            is_not(const condition_type& condition) : condition_holder(condition){}
            bool send() const  {return !condition_holder.send();}
            private:
            condition_type condition_holder;
        };

        ///A general condition which sends a message when a predicate returns 
        ///true
        ///
        ///This condition can be used to send a message when a simple predicate 
        ///is true. the predicate must be of a simple signiture of 
        ///"bool predicate()" where takes no parameters and returns a boolean.
        template <class predicate_type>
            struct if_true : public interface
        {
            if_true(const predicate_type& predicate) : predicate_holder(predicate) {}
            bool send() const {return predicate_holder();}
            private:
            predicate_type predicate_holder;
        };
    }
}//namespace message
}//namespace controller
}//namespace ncc
#endif

