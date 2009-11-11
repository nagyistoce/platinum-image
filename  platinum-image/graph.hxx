//////////////////////////////////////////////////////////////////////////
//
//  graph.h $Revision $
//
//
//  $LastChangedBy $
//

// This file is part of the Platinum library.
// Copyright (c) 2007 Uppsala University.
//
//    The Platinum library is free software; you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    The Platinum library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with the Platinum library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


#ifndef __graph__
#define __graph__

template<class ELEMTYPE>
class node
{
public:
	node<ELEMTYPE>() {};
	node<ELEMTYPE>(short label, ELEMTYPE value) {
		_label=label;	
		_value=value;
	}
	void add_arc(node<ELEMTYPE>* end_node) {arcs.insert(end_node);}
	void add_arcs(set<node<ELEMTYPE>* > end_nodes) {
		typename set<node<ELEMTYPE>* >::iterator i = end_nodes.begin();
		while (i!=end_nodes.end()) {
			arcs.insert(*i);
			i++;
		}
	}
	void remove_arc(node<ELEMTYPE>* end_node) {arcs.erase(end_node);}
	bool arc_exists(node<ELEMTYPE>* end_node) {return arcs.find(end_node)!=arcs.end();}
	short get_label() {return _label;}
	ELEMTYPE get_value() {return _value;}
	set<node<ELEMTYPE>* > get_arcs() {return arcs;}
	void set_value(ELEMTYPE value) {_value=value;}
	void print(){
		cout << "Node " << _label << ": value=" << _value << ", arcs to nodes: ";
		typename set<node<ELEMTYPE>* >::iterator i = arcs.begin();
		while (i!=arcs.end()) {
			cout << (*i)->get_label() << " ";
			i++;
		}
		cout << endl;
	}
private:
	short _label;
	ELEMTYPE _value; // could be replaced with a pointer if a larger data structure is used
	set<node<ELEMTYPE>* > arcs;
};

template<class ELEMTYPE> 
class graph
{
public:
	graph<ELEMTYPE>() {};
	graph<ELEMTYPE>(map<short, node<ELEMTYPE>* > nodes) {_nodes = nodes;}
	void add_node(short label, ELEMTYPE value) {
		_nodes[label]=new node<ELEMTYPE>(label, value);	
	}
	node<ELEMTYPE>* get_node(short label) {return _nodes[label];}
	void link_nodes(short node1, short node2) {
		pt_error::error_if_false(_nodes.find(node1)!=_nodes.end() && _nodes.find(node2)!=_nodes.end(), "Cannot link nodes that are not in graph", pt_error::debug);
		_nodes[node1]->add_arc( _nodes[node2] );
		_nodes[node2]->add_arc( _nodes[node1] );
	}
	map<ELEMTYPE, vector<node<ELEMTYPE>* > > get_value_to_node_map() { // redundant??
		map<ELEMTYPE, vector<node<ELEMTYPE>* > > value_map;
		typename map<short, node<ELEMTYPE>* >::iterator i = _nodes.begin();
		while (i!=_nodes.end()) {
			value_map[i->second->get_value()].push_back(i->second);
			i++;
		}
		return value_map;
	}
	set<ELEMTYPE> get_values() {
		set<ELEMTYPE> value_set;
		typename map<short, node<ELEMTYPE>* >::iterator i = _nodes.begin();
		while (i!=_nodes.end()) {
			value_set.insert(i->second->get_value());
			i++;
		}
		return value_set;
	}
	void merge_nodes(short staying, short going) {
		_nodes[staying]->add_arcs(_nodes[going]->get_arcs());
		typename map<short, node<ELEMTYPE>* >::iterator map_iter = _nodes.begin();
		while (map_iter!=_nodes.end()) {
			if (map_iter->second->arc_exists(_nodes[going])){
				map_iter->second->add_arc(_nodes[staying]);
				map_iter->second->remove_arc(_nodes[going]);
			}
			map_iter++;
		}
		if(_nodes[staying]->arc_exists(_nodes[staying]))
			_nodes[staying]->remove_arc(_nodes[staying]);
		_nodes.erase(going);
	}
	bool node_exists(short label) {return _nodes.find(label)!=_nodes.end();}
	void print() {
		typename map<short, node<ELEMTYPE>* >::iterator i = _nodes.begin();
		while (i!=_nodes.end()) {
			i->second->print();
			i++;
		}
	}
private:
	map<short, node<ELEMTYPE>* > _nodes;
};

#endif