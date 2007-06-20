// $Id: points_seq_func1D.hxx 

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

#include "points_seq_func1D.h"

template <class KEY_TYPE, class VALUE_TYPE>
points_seq_func1D<KEY_TYPE, VALUE_TYPE>::points_seq_func1D(KEY_TYPE key_min, VALUE_TYPE val_min, KEY_TYPE key_max, VALUE_TYPE val_max, int nr_points)
{
	cout<<"Constructor - points_seq_func1D"<<endl; //JK1

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
		map<KEY_TYPE,VALUE_TYPE>::iterator the_iterator;
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

template <class KEY_TYPE, class VALUE_TYPE>
void points_seq_func1D<KEY_TYPE, VALUE_TYPE>::set_data(KEY_TYPE this_key, KEY_TYPE new_k, VALUE_TYPE new_v, KEY_TYPE k_min=NULL, VALUE_TYPE v_min=NULL, KEY_TYPE k_max=NULL, VALUE_TYPE v_max=NULL)
{
	//JK3 Note that currently, there is no checking if new_k already excists...
	if(this_key == new_k)
	{
		themap[this_key] = new_v;
	}
	else
	{
		//make sure values dont go outside given boudaries...
		if(k_min!=NULL){if(new_k<k_min){new_k=k_min;}}		
		if(v_min!=NULL){if(new_v<v_min){new_v=v_min;}}
		if(k_max!=NULL){if(new_k>k_max){new_k=k_max;}}
		if(v_max!=NULL){if(new_v>v_max){new_v=v_max;}}

		erase(this_key);
		insert(new_k,new_v);
	}
}

template <class KEY_TYPE, class VALUE_TYPE>
bool points_seq_func1D<KEY_TYPE, VALUE_TYPE>::is_occupied(KEY_TYPE k)
{
	return (themap[k]!=NULL);
}


template <class KEY_TYPE, class VALUE_TYPE>
float points_seq_func1D<KEY_TYPE, VALUE_TYPE>::find_dist_to_closest_point2D(KEY_TYPE x, VALUE_TYPE y, KEY_TYPE &closest_key)
{
	float min_dist=1000000;
	float this_dist=0;
	float a=0;
	float b=0;
	map<KEY_TYPE,VALUE_TYPE>::iterator the_iterator;
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

	map<KEY_TYPE,VALUE_TYPE>::iterator the_iterator;

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
	cout<<"get_spline_derivatives..."<<endl;

	float *x = new float[100];
	float *y = new float[100];
	float *y2 = new float[100];

	int i=1; //Note: index 1...n used in Numerical Recipies...
	map<KEY_TYPE,VALUE_TYPE>::iterator the_iterator;
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

	map<KEY_TYPE,VALUE_TYPE>::iterator the_iterator;

	//if the this_x-value is smaller/larger than the first/last --> return first/last...
	//first
	the_iterator = themap.begin();
	if(this_x < the_iterator->first){return the_iterator->second;}

	//last
	for (int i=0;i<size()-1;i++)
		the_iterator++;
	if(this_x >= the_iterator->first){return the_iterator->second;}

	float *x = new float[100];
	float *y = new float[100];

	int i=1;	//Note: index 1...n used in Numerical Recipies...
	for (the_iterator = themap.begin(); the_iterator != themap.end(); the_iterator++) 
	{
		x[i] = float(the_iterator->first);
		y[i] = float(the_iterator->second);
		i++;
	}

	float ret = pt_splint1D(x,y,y2,size(),this_x);

	delete x;
	delete y;
	return ret;
}




template <class KEY_TYPE, class VALUE_TYPE>
void points_seq_func1D<KEY_TYPE, VALUE_TYPE>::printdata()
{
	cout<<endl<<"printdata()"<<endl;
	map<KEY_TYPE,VALUE_TYPE>::iterator the_iterator;
	for (the_iterator = themap.begin(); the_iterator != themap.end(); the_iterator++) 
	{
		cout<<"x:"<<the_iterator->first<<"\t"<<"y:"<<the_iterator->second<<endl;
	}
	cout<<".............."<<endl;
}