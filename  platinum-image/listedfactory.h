//////////////////////////////////////////////////////////////////////////
//
//  Listedfactory $Revision$
//
//  Object factory class used to keep track of interchangeable objects
//  of one particular class, like renderers or transfer functions
//
//  Adapted from an article by Robert Geiman
//  http://www.gamedev.net/reference/articles/article2097.asp
//
//  $LastChangedBy$

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

#ifndef __listedfactory__
#define __listedfactory__

#include <FL/Fl_Menu.h>
#include <FL/Fl_Button.h>
#include <map>
#include "FLTKutilities.h"

typedef std::string factoryIdType; //JK


//The syntax for using this final implementation of the object factory looks like this:
//
//ObjectFactory<Shape *(int), std::string> shape_factory;
//
//shape_factory.Register<Triangle>("triangle");
//shape_factory.Register<Square>("square");
//
//
//Shape *shape1 = shape_factory.Create("triangle", 10);
//Shape *shape2 = shape_factory.Create("square", 20);


template<typename BaseClassType, typename ClassType>
BaseClassType *CreateObject()
{
    return new ClassType;
}


template<typename BaseClassType>
class listedfactory
{
protected:
    typedef BaseClassType *(*CreateObjectFunc)(); //JK
//    typedef BaseClassType *(*CreateObjectFunc)(int x,int y,int w,int h);
    std::map<factoryIdType, CreateObjectFunc> m_object_creator;
    
public:
	listedfactory()
	{
	    //cout<<"listedfactory..."<<endl;
	    m_object_creator = std::map<factoryIdType, CreateObjectFunc>();
	}

    typedef typename std::map<factoryIdType, CreateObjectFunc>::const_iterator ConstIterator;
    typedef typename std::map<factoryIdType, CreateObjectFunc>::iterator Iterator;
    
    class lf_menu_params
    {
	public:
		void *receiver;
        factoryIdType type;
            
        lf_menu_params(void *r,factoryIdType t)
        {
			receiver = r;
            type = t;
		}
//        BaseClassType *Create()  //JK2 - Could not make this work... "could not call a non-static function" 
//        {
//			return listedfactory::Create(type);
//        }
	};
    
    template<typename ClassType>
    bool Register(factoryIdType unique_id = "")
    {
		if (unique_id == "")
			{ unique_id = ClassType::typekey(); }

//		cout<<unique_id + "-->m_object_creator.size()="<<endl;
		
		if (m_object_creator.find(unique_id) != m_object_creator.end())
			return false;
            
		m_object_creator[unique_id] = &CreateObject<BaseClassType, ClassType>;
        return true;
	}
    
    bool Unregister(factoryIdType unique_id)
	{
        return (m_object_creator.erase(unique_id) == 1);
	}
    
//    static BaseClassType *Create(factoryIdType unique_id)
    BaseClassType *Create(factoryIdType unique_id)
//    BaseClassType *Create(factoryIdType unique_id, int x, int y, int w, int h)
    {
        Iterator iter = m_object_creator.find(unique_id);
        
        if (iter == m_object_creator.end())
            return NULL;
        
        return ((*iter).second)();
//        return ((*iter).second)(x, y, w, h);
    }
    
    ConstIterator begin() const{return m_object_creator.begin();}
    Iterator begin(){return m_object_creator.begin();}
    ConstIterator end() const{return m_object_creator.end();}
    Iterator end(){return m_object_creator.end();}


    //typename Iterator::SizeType num_items ()
    typename std::map<factoryIdType, CreateObjectFunc>::size_type num_items()
	{
        return m_object_creator.size();
    }
    

    Fl_Menu_Item * createmenu (Fl_Callback * cb, void * receiver)
    {    
        Fl_Menu_Item * fmenu;
        int num_items = m_object_creator.size(); 
        fmenu = new Fl_Menu_Item [num_items+1];
        int m = 0;
        
        for (ConstIterator i = m_object_creator.begin();i != m_object_creator.end();i++)
            {            
            init_fl_menu_item(fmenu[m]);
            
            fmenu[m].label(i->first.c_str());
            fmenu[m].callback(cb);
            fmenu[m].user_data(new lf_menu_params(receiver,(i->first)));
            fmenu[m].flags = FL_MENU_RADIO;
            
            m++;
            }
        
        fmenu[num_items].label(NULL);
        return fmenu;
        }
    
    void buttons (Fl_Group * box,bool horizontal,Fl_Callback * cb)
    {
        int buttonSize  = horizontal? box->h():box->w();
        int x = box->x();
        int y = box->y();
        box->begin();

        for (ConstIterator i = m_object_creator.begin();i != m_object_creator.end();i++)
            {
            std::string name = i->first;
            Fl_Button * button = new Fl_Button (x,y,buttonSize,buttonSize, name.substr(0,1).c_str());
			//button->label(strdup(name.substr(0,1).c_str()));
			//button->tooltip(strdup(name.c_str()));
			//button->label(name.substr(0,1).c_str());		//only first letter is used		
			button->tooltip(name.c_str());					//the full name can be seen in the tooltip
            
            button->box(FL_UP_BOX);
            button->down_box(FL_DOWN_BOX);
            button->type(FL_RADIO_BUTTON);
            button->callback(cb);
            button->user_data(reinterpret_cast<const void*>(&(i->first)));
            
            if (horizontal)
                { x += buttonSize; }
            else
                { y += buttonSize; }
            }
        
        box->size(x,y);
        box->end();
    }
};


//---------------------------------
//---------------------------------
/*
class FLTKpane; //(int,int,int,int);

class viewportfactory:listedfactory<FLTKpane,FLTKpane *(*CreatePaneFunc)(int,int,int,int)>
{

protected:
	//typedef FLTKpane *(*CreatePaneFunc)(int,int,int,int);
    
public:

	FLTKpane *Create(factoryIdType unique_id, int x, int y, int w, int h)
    {
        Iterator iter = m_object_creator.find(unique_id);
        
        if (iter == m_object_creator.end())
            return NULL;
        
//        return ((*iter).second)(int,int,int,int);
        return ((*iter).second)(x,y,w,h);
//        return ((*iter).second(x,y,w,h))();
//        return ((*iter).second)();				//JK2
    }


//	CreatePaneFunc f = &MPRPane;
//	(CreatePaneFunc*)(1,2,3,4);
};
*/





//---------------------------------
//---------------------------------
class panefactory //! transfer gets its own object factory type because constructors for templated classes cannot be stored
{
protected:
    int num_items;
    static const std::string pane_names[];

public:
	panefactory()
	{
    num_items = 0;
    while  (pane_names[num_items] != "")
        { num_items++; }
    }

	~panefactory(){}
 /*   
    class tf_menu_params {
			image_base * image;
			std::string type;
		public:
			tf_menu_params (const std::string t,image_base * i);
			void switch_tf();
    };
    
    template <class ELEMTYPE >
        transfer_scalar_base<ELEMTYPE > * Create(factoryIdType unique_id, image_storage<ELEMTYPE >* s);
 */   
    Fl_Menu_Item* get_FLTK_menu(Fl_Callback *cb)
    {
    Fl_Menu_Item * fmenu;
    fmenu = new Fl_Menu_Item [num_items+1];

    for (int m=0; m < num_items; m++)
        {
        init_fl_menu_item(fmenu[m]);

        fmenu[m].label(pane_names[m].c_str());
        fmenu[m].callback(cb);
        //fmenu[m].argument(m);
        fmenu[m].flags = FL_MENU_RADIO;
        }

    fmenu[num_items].label(NULL);

    return fmenu;
	}

    
//    static const std::string tf_name(int);

};


#endif