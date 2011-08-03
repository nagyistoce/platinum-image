//////////////////////////////////////////////////////////////////////////////////
//
//  shape_calc $Revision: 706 $
///
/// 
///
//  $LastChangedBy: joel.kullberg $

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


#include "shape_calc.h"

int shape_calc::sgn ( long a )
{
	if (a > 0)
		{ return +1; }
	else if (a < 0) 
		{ return -1; }
	else
		{ return 0; }
}

vector<Vector3D> shape_calc::calc_cirlce_2d(Vector3D center, int radius){
	vector<Vector3D> vec; 
	int x0 = center[0];
	int y0 = center[1];

    int f = 1 - radius;
    int ddF_x = 1;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

	vec.push_back(create_Vector3D(x0,y0+radius,1));
	vec.push_back(create_Vector3D(x0,y0-radius,1));
	vec.push_back(create_Vector3D(x0+radius,y0,1));
	vec.push_back(create_Vector3D(x0-radius,y0,1));
 
    while(x < y)
    {
		// ddF_x == 2 * x + 1;
		// ddF_y == -2 * y;
		// f == x*x + y*y - radius*radius + 2*x - y + 1;
		if(f >= 0){
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;  
		// draw following circle points   
		vec.push_back(create_Vector3D(x0+x,y0+y,1));
		vec.push_back(create_Vector3D(x0+x,y0-y,1));
		vec.push_back(create_Vector3D(x0-x,y0+y,1));
		vec.push_back(create_Vector3D(x0-x,y0-y,1));

		vec.push_back(create_Vector3D(x0+y,y0+x,1));
		vec.push_back(create_Vector3D(x0+y,y0-x,1));
		vec.push_back(create_Vector3D(x0-y,y0+x,1));
		vec.push_back(create_Vector3D(x0-y,y0-x,1));  
		x++;  
	}  
	return vec;
}
vector<Vector3D> shape_calc::calc_line_2d(Vector3D start, Vector3D stop){
	vector<Vector3D> vec;
	int a,b,c,d;
	a = start[0];
	b = start[1];
	c = stop[0];
	d = stop[1];

	long u, s, v, d1x, d1y, d2x, d2y, m, n;
	int  i;
	
	u = c - a;
	v = d - b;
	
	d1x = sgn(u);
	d1y = sgn(v);
	
	d2x = sgn(u);
	d2y = 0;
	
	m = abs(u);
	n = abs(v);
	
	if ( m <= n )
	{
		d2x = 0;
		d2y = sgn(v);
		m = abs(v);
		n = abs(u);
	}
	
	s = (int) (m / 2);
	
	for ( i=0; i < round(m); i++ )
	{
		//Can not check if in veiwport here!
	//	if ( a >= 0 && a <= sx && b >= 0 && b <= sy )
	//	{	// inside the viewport
			vec.push_back(create_Vector3D(a,b,1));
	//	}  
	
		s += n;
		if (s >= m)
		{
		  s -= m;
		  a += d1x;
		  b += d1y;
		}
		else 
		{
		  a += d2x;
		  b += d2y;
		}
	}

	return vec;
}
vector<Vector3D> shape_calc::calc_gauss_2d(Vector3D sigma, Vector3D my, double amplitude){
	vector<Vector3D> vec;
	return vec;
}

vector<Vector3D> shape_calc::calc_line_3d(Vector3D start, Vector3D stop){
	vector<Vector3D> vec;
	
	/*start[0] = floor(start[0]);
	start[1] = floor(start[1]);
	start[2] = floor(start[2]);

	stop[0] = ceil(stop[0]);
	stop[1] = ceil(stop[1]);
	stop[2] = ceil(stop[2]);*/
	Vector3D diff = (stop - start);
	float n = diff.GetNorm();//max( max(abs(diff[0]),abs(diff[1])), abs(diff[2]) );
	Vector3D delta;
	delta[0] = diff[0]/(n+1.0f);
	delta[1] = diff[1]/(n+1.0f);
	delta[2] = diff[2]/(n+1.0f);

	for(float i=0;i<n+1;i++){
		vec.push_back(create_Vector3D(start[0]+delta[0]*i, start[1]+delta[1]*i, start[2]+delta[2]*i));
	}
	return vec;
}
vector<Vector3D> shape_calc::calc_rect_3d(Vector3D c1, Vector3D c2, Vector3D c3, Vector3D c4){
	vector<Vector3D> vec;
	vector<Vector3D> temp;

	vec = calc_line_3d(c1, c2);
	//vec.pop_back(); //else we get double points here...
	
	temp = calc_line_3d(c2, c3);
	//temp.pop_back();
	vec.insert(vec.end(),temp.begin(),temp.end());
	temp.clear();

	temp = calc_line_3d(c3, c4);
	//temp.pop_back();
	vec.insert(vec.end(),temp.begin(),temp.end());
	temp.clear();

	temp = calc_line_3d(c4, c1);
	//temp.pop_back();
	vec.insert(vec.end(),temp.begin(),temp.end());

	temp.clear();
	return vec;
}


vector<Vector3D> shape_calc::calc_cirlce_3d(Vector3D c, Vector3D n, float radius){
	vector<Vector3D> vec;

	double factor;
	Vector3D w,u,v;
	double pi = pt_PI; //Used own definition before...
	
	w = n;
	//Taken from http://www.gamedev.net/community/forums/topic.asp?topic_id=427311 and modified to draw ellipses
	if (abs(w[0]) >= abs(w[1])){
		factor = 1/sqrt(w[0]*w[0]+w[2]*w[2]);
		u[0] = -w[2]*factor;
		u[1] = 0;
		u[2] = w[0]*factor;
	}else{
		factor = 1/sqrt(w[1]*w[1]+w[2]*w[2]);
		u[0] = 0;
		u[1] = w[2]*factor;
		u[2] = -w[1]*factor;
	}
	v[0] = w[1]*u[2] - w[2]*u[1];
	v[1] = w[2]*u[0] - w[0]*u[2];
	v[2] = w[0]*u[1] - w[1]*u[0];
 
	double step_length = (2*pi)/(radius*10);
	for(double i = 0.0; i <= 2*pi+step_length; i+=step_length){
		Vector3D point;
		point[0] = c[0] + (radius*cos(i))*u[0] + (radius*sin(i))*v[0];
		point[1] = c[1] + (radius*cos(i))*u[1] + (radius*sin(i))*v[1];
		point[2] = c[2] + (radius*cos(i))*u[2] + (radius*sin(i))*v[2];
		vec.push_back(point);
	}

	return vec;
}

