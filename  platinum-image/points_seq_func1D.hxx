// $Id$

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

#include <iostream>
#include <math.h>

template <class KEY_TYPE, class VALUE_TYPE>
points_seq_func1D<KEY_TYPE, VALUE_TYPE>::points_seq_func1D(KEY_TYPE key_min, VALUE_TYPE val_min, KEY_TYPE key_max, VALUE_TYPE val_max, int nr_points)
{

	float dx=float(key_max-key_min)/float(nr_points);
	float dy=float(val_max-val_min)/float(nr_points);
	for(float x=key_min; x<=key_max; x +=dx)
	{
		themap[x] = float(x - key_min)*dy/dx;
	}
}

template <class KEY_TYPE, class VALUE_TYPE>
void points_seq_func1D<KEY_TYPE, VALUE_TYPE>::insert(KEY_TYPE key, VALUE_TYPE val)
{
	themap[key] = val;
}

template <class KEY_TYPE, class VALUE_TYPE>
void points_seq_func1D<KEY_TYPE, VALUE_TYPE>::erase(KEY_TYPE key)
{
	themap.erase(key);
}

template <class KEY_TYPE, class VALUE_TYPE>
typename std::map<KEY_TYPE,VALUE_TYPE>::iterator points_seq_func1D<KEY_TYPE, VALUE_TYPE>::begin()
    {
    return themap.begin();
    }

template <class KEY_TYPE, class VALUE_TYPE>
typename std::map<KEY_TYPE,VALUE_TYPE>::iterator points_seq_func1D<KEY_TYPE, VALUE_TYPE>::end()
    {
    return themap.end();
    }

template <class KEY_TYPE, class VALUE_TYPE>
unsigned int points_seq_func1D<KEY_TYPE, VALUE_TYPE>::size()
{
	return themap.size();
}

template <class KEY_TYPE, class VALUE_TYPE>
void points_seq_func1D<KEY_TYPE, VALUE_TYPE>::get_data(unsigned int map_index, KEY_TYPE &k, VALUE_TYPE &v)
{
	k = NULL;
	v = NULL;
	if(map_index>=0 && map_index<size())
        {
        typename std::map< KEY_TYPE , VALUE_TYPE >::iterator the_iterator;
        
		the_iterator = themap.begin();
		for(unsigned int i = 1;i<=map_index;i++)
            {
			the_iterator++;
            }
		k = the_iterator->first;
		v = the_iterator->second;
        }
}

template <class KEY_TYPE, class VALUE_TYPE>
VALUE_TYPE points_seq_func1D<KEY_TYPE, VALUE_TYPE>::get_value(KEY_TYPE k)
{
	return themap[k];
}

//template <class KEY_TYPE, class VALUE_TYPE>
//void points_seq_func1D<KEY_TYPE, VALUE_TYPE>::set_data(KEY_TYPE this_key, KEY_TYPE new_k, VALUE_TYPE new_v, KEY_TYPE k_min, VALUE_TYPE v_min, KEY_TYPE k_max, VALUE_TYPE v_max)
template <class KEY_TYPE, class VALUE_TYPE>
void points_seq_func1D<KEY_TYPE, VALUE_TYPE>::set_data(KEY_TYPE this_key, KEY_TYPE new_k, VALUE_TYPE new_v)
{
#ifdef _DEBUG
    //std::cout<<"set_data"<<this_key<<"\t"<<new_k<<"\t"<<new_v<<std::endl;
#endif
    /*	
	//make sure values dont go outside given boudaries...
	if(v_max!=NULL){
		if(new_k<k_min)
			new_k=k_min;
		if(new_v<v_min)
			new_v=v_min;
		if(new_k>k_max)
			new_k=k_max;
		if(new_v>v_max)
			new_v=v_max;
	}
*/
	if(this_key==new_k)
	{
		themap[this_key]=new_v;
	}else{
		erase(this_key);
		insert(new_k,new_v);	//ordered data --> O(n)
	}
}

template <class KEY_TYPE, class VALUE_TYPE>
bool points_seq_func1D<KEY_TYPE, VALUE_TYPE>::is_occupied(KEY_TYPE k)
{
	return (themap[k] != 0);
}


template <class KEY_TYPE, class VALUE_TYPE>
float points_seq_func1D<KEY_TYPE, VALUE_TYPE>::find_dist_to_closest_point2D(KEY_TYPE x, VALUE_TYPE y, KEY_TYPE &closest_key)
{
	float min_dist=1000000;
	float this_dist=0;
	float a=0;
	float b=0;
    typename std::map<KEY_TYPE,VALUE_TYPE>::iterator the_iterator;
	for (the_iterator = themap.begin(); the_iterator != themap.end(); the_iterator++) 
	{
		a = float(the_iterator->first  - x);
		b = float(the_iterator->second - y);
		this_dist = sqrt(a*a+b*b);
		if(this_dist<min_dist)
		{
			min_dist=this_dist;
			closest_key = the_iterator->first;
		}
	}
	return min_dist;
}


template <class KEY_TYPE, class VALUE_TYPE>
double points_seq_func1D<KEY_TYPE, VALUE_TYPE>::get_value_interp_linear(double x)
{	
	double x1,y1,x2,y2;

    typename std::map<KEY_TYPE,VALUE_TYPE>::iterator the_iterator;

	//if the x-value is smaller/larger than the first/last --> return first/last...
	//first
	the_iterator = themap.begin();
	if(x < the_iterator->first){return the_iterator->second;}

	//last
	for (int i=0;i<size()-1;i++)
		the_iterator++;
	if(x >= the_iterator->first){return the_iterator->second;}

	for (the_iterator = themap.begin(); the_iterator != themap.end(); the_iterator++) 
	{
		if(the_iterator->first > x) //means that we just found the value right of x.
		{
			x2 = the_iterator->first;
			y2 = the_iterator->second;
			the_iterator--;
			x1 = the_iterator->first;
			y1 = the_iterator->second;
			break;
		}
	}

	return y1 + double(y2-y1)/double(x2-x1)*(double(x)-x1); // y1 + dy/dx(=k)*x...
}


template <class KEY_TYPE, class VALUE_TYPE>
float* points_seq_func1D<KEY_TYPE, VALUE_TYPE>::get_spline_derivatives()
{
#ifdef _DEBUG
    //std::cout<<"get_spline_derivatives..."<<std::endl;
#endif

	float *x = new float[size()+2];
	float *y = new float[size()+2];
	float *y2 = new float[size()+2];

	int i=1; //Note: index 1...n used in Numerical Recipies...
    typename std::map<KEY_TYPE,VALUE_TYPE>::iterator the_iterator;
	for (the_iterator = themap.begin(); the_iterator != themap.end(); the_iterator++) 
	{
		x[i] = float(the_iterator->first);
		y[i] = float(the_iterator->second);
//		cout<<x[i]<<" "<<y[i]<<endl;
		i++;
	}
	

//	pt_spline1D(x,y,size(),(y[1]-y[0])/(x[1]-x[0]),(y[i]-y[i-1])/(x[i]-x[i-1]),y2);
	pt_spline1D(x,y,size(),0,0,y2);

	delete x;
	delete y;
	return y2;
}




template <class KEY_TYPE, class VALUE_TYPE>
float points_seq_func1D<KEY_TYPE, VALUE_TYPE>::get_value_interp_spline(float this_x, float y2[])
{
    typename std::map<KEY_TYPE,VALUE_TYPE>::iterator the_iterator;

	//if the this_x-value is smaller/larger than the first/last --> return first/last...

	//first
	the_iterator = themap.begin();
	if(this_x < the_iterator->first){return the_iterator->second;}

	//last
	for (int i=0;i<size()-1;i++)
		the_iterator++;
	if(this_x >= the_iterator->first){return the_iterator->second;}


	float *x = new float[themap.size()+2];
	float *y = new float[themap.size()+2];

//	int i;	//Note: i -- index 1...n used in Numerical Recipies...
	the_iterator = themap.begin();
	for(int i=1; the_iterator != themap.end(); the_iterator++, i++) 
	{
		x[i] = float(the_iterator->first);
		y[i] = float(the_iterator->second);
//		i++;
	}

	float ret = pt_splint1D(x,y,y2,size(),this_x);

	if(ret > std::numeric_limits<VALUE_TYPE>::max())
	{ret = std::numeric_limits<VALUE_TYPE>::max();}

	if(ret < std::numeric_limits<VALUE_TYPE>::min())
	{ret = std::numeric_limits<VALUE_TYPE>::min();}

	delete x;
	delete y;
	return ret;
}




template <class KEY_TYPE, class VALUE_TYPE>
void points_seq_func1D<KEY_TYPE, VALUE_TYPE>::printdata()
{
    std::cout<<std::endl<<"printdata()"<<std::endl;
    typename std::map<KEY_TYPE,VALUE_TYPE>::iterator the_iterator;
	for (the_iterator = themap.begin(); the_iterator != themap.end(); the_iterator++) 
	{
        std::cout<<"x:"<<the_iterator->first<<"\t"<<"y:"<<the_iterator->second<<std::endl;
	}
    std::cout<<".............."<<std::endl;
}
