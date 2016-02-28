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

#ifndef NCCENTRIFUGE_TREE_H
#define NCCENTRIFUGE_TREE_H

#include <algorithm>
#include <list>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
namespace ncc {
namespace tree
{
    ///Allows any class to become a node to a tree structure. 
    ///
    ///Inherent this class to turn your class into a node in a tree structure. 
    ///Example:
    ///		- class MyClass : public ncc::is_node<MyClass>
    template <typename data_type>
        class is_node : public boost::enable_shared_from_this<data_type> , boost::noncopyable
    {
        public:
            typedef boost::shared_ptr<data_type> node_type;
            typedef boost::shared_ptr<data_type const> const_node_type;
            typedef boost::weak_ptr<data_type> observer_node_type;
            typedef std::list<node_type> child_list_type;
            typedef typename child_list_type::iterator child_iterator;
            typedef typename child_list_type::const_iterator const_child_iterator;
            typedef typename child_list_type::size_type child_size_type;

            ///Returns the parent of the current node.
            ///@{
            const data_type& parent() const {return *parent_ptr;}
            data_type& parent() { return *parent_ptr;}		
            ///@}

            ///Returns a boost::shared_ptr of the current node.
            ///@{	          
            node_type self(){return this->shared_from_this();}
            const_node_type self() const {return this->shared_from_this();}
            ///@}

            ///Returns a list of children.
            ///@{
            virtual const child_list_type& children() const { return child_list;}
            virtual child_list_type& children() { return child_list;}
            ///@}

            ///Adds a node as a child of the current node.
            virtual bool add_child(node_type new_child);

            ///Removes a child from the node.
            virtual bool remove_child(node_type target_child);

            ///The only valid constructor.
            ///
            ///This constructor takes a pointer of the child class which is the node.
            ///Example:
            ///         - Myclass() : is_node(this) {}
            ///
            ///Where Myclass() is a constructor of "class MyClass : public is_node<MyClass>"
            is_node(data_type* derived_this_ptr) : parent_ptr(0), this_ptr(derived_this_ptr), child_list(){}
            virtual ~is_node(){};

            //stl type abstract_interface
            ///Returns the begining iterator to the children.
            ///@{
            child_iterator children_begin() {return child_list.begin();}
            const_child_iterator children_begin() const {return child_list.begin();}
            ///@}

            ///Returns the end interator to the children.
            ///@{
            child_iterator children_end() { return child_list.end();}
            const_child_iterator children_end() const {return child_list.begin();}
            ///@}

            ///Returns the number of children.
            child_size_type children_size() const {return child_list.size();}

        private:
            data_type* parent_ptr;
            data_type* this_ptr;
            child_list_type child_list;
    };

    template <class data_type>
        bool
        is_node<data_type>::add_child(typename is_node::node_type new_child)
        {
            if(!new_child) return false;
            //make sure that the pointer does not exist already
            typename child_list_type::const_iterator child_exists = std::find(child_list.begin(), child_list.end(), new_child);
            if(child_exists != child_list.end()) return false; 		

            child_list.push_back(new_child);
            new_child->parent_ptr = this_ptr;	
            return true;
        }
    template <class data_type>
        bool 
        is_node<data_type>::remove_child(typename is_node::node_type target_child)
        {
            //I could have used child_list_type_list.remove(target_child)
            //but then I could not return a boolean if the child was removed or not.
            //because std::list::remove returns void.
            //so we will do it the classic way and find the target_child and remove it using erase.
            //note that because add_child guarantees that there is no duplicate child.
            typename child_list_type::iterator target = std::find(child_list.begin(), child_list.end(), target_child);
            if(target != child_list.end()) 
            {
                child_list.erase(target);
                return true;
            }
            return false;
        }
    namespace hidden
    {
        template<class node_type, class predicate_type>
            node_type depth_first_search(node_type& root, 
                    predicate_type& predicate)
            {
                if(!root) return node_type();    //if the root ptr is 0, then we 
                //return 0.
                if(predicate(root)) return root; //if the root satisfies the 
                //predicate then we found it and 
                //return the root.
                //for each unvisited child we search again.
                typename node_type::element_type::child_iterator end = root->children_end();
                for(typename node_type::element_type::child_iterator child = root->children_begin(); child != end; ++child)
                    if(node_type found_node = hidden::depth_first_search(*child, predicate))
                        return found_node;
                return node_type();
            }

        template <class node_type, class visitor_type>
            void transverse_depth_first(node_type& root, 
                    visitor_type& visit)
            {
                if(!root) return;	//if we have a null pointer then return and don't 
                //do anything.
                if(visit(root) == false) return; //if visit returns false then the 
                //children are not visited.                            
                //visit each child if it has not been visited
                typename node_type::element_type::child_iterator end = root->children_end();
                for(typename node_type::element_type::child_iterator child = root->children_begin(); child != end; ++child)
                    hidden::transverse_depth_first(*child, visit);
            }

        template <class node_type, class predicate>
            void remove_if(node_type& root, 
                    predicate& should_remove)
            {
                if(!root) return; //if we have a null pointer then return and don't
                //do anything
                //visit each child if it has not been visited
                typename node_type::element_type::child_iterator end = root->children_end();
                for(typename node_type::element_type::child_iterator child = root->children_begin(); child != end ;)
                {
                    if(should_remove(*child))
                    {
                        child = root->children().erase(child); continue;
                    }
                    else
                        hidden::remove_if(*child, should_remove);
                    ++child;
                }
            }//remove_if
    }//namespace hidden


    ///This class takes a root node and does a depth first search using the
    ///specifiied predicate.
    ///
    ///This function can be used to search through the tree. If the predicate 
    ///returns true, than the search is complete.
    ///@param root The root node to start the search.
    ///@param predicate The predicate takes a const node_type and returns true
    ///or false depending on the logic. 
    ///@return The found node.
    template<class node_type, class predicate_type>
        inline node_type depth_first_search(node_type& root, predicate_type predicate)
        {
            return hidden::depth_first_search(root, predicate);
        }

    ///This function transverses the tree in depth first order and visits each
    ///node.
    ///
    ///Use this function to do something to each node in the tree. This function
    ///tranverses the tree and applies the visitor to each node. The visitor
    ///takes a node_type and returns true or false. If it returns true then the
    ///transversal is continued, else the tranversal stops.
    ///@param root The root node to start transversing
    ///@param visit The visitor which takes a node_type and returns true or false.
    template <class node_type, class visitor_type>
        inline void transverse_depth_first(node_type& root, visitor_type visit)
        {
            hidden::transverse_depth_first(root, visit);
        }

    ///This function transverses the tree and removes if the predicate returns 
    ///true.
    ///
    ///This function is useful for removing nodes depending on a predicate.
    ///The transversal is done in depth first order. 
    ///@param The root node to start removing.
    ///@param The predicate which takes a node_type and returns true or false.
    template <class node_type, class predicate>
        inline void remove_if(node_type& root, predicate should_remove)
        {
            hidden::remove_if(root, should_remove);
        }

}//tree
}//namespace ncc
#endif

