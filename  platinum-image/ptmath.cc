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


#include "ptmath.h"
#include "error.h"
#include <vcl_iostream.h>
#include <vnl/vnl_gamma.h>



gaussian::gaussian(float amp, float cent, float sig){
	amplitude = amp;
	center = cent;
	sigma = sig;
}

gaussian::~gaussian(){}

float gaussian::evaluate_at(float x){
	return amplitude * exp( -0.5 * pow((x-center)/sigma,2) );
}

double gaussian::integrate_total_area(int from_x, int to_x){
	double area=0;
	for(int i=from_x;i<=to_x;i++){
		area += evaluate_at(i);
	}
	return area;
}

int gaussian::get_x_that_includes_area_fraction(double fraction, int from_x, int to_x){
	double area=0;
	double tot_area = this->integrate_total_area(from_x, to_x);
	for(int i=from_x;i<=to_x;i++){
		area += evaluate_at(i);
		if(area/tot_area > fraction){
			return i;
		}
	}
	return to_x;
}

float gaussian::get_value_at_intersection_between_centers(gaussian g2, float num_calc_steps)
{
	float from = min(this->center,g2.center);
	float to = max(this->center,g2.center);
	float min_diff = std::numeric_limits<float>::max();
	float min_diff_pos = std::numeric_limits<float>::max();
	float diff;

	for(float i=from; i<=to; i += (to-from)/num_calc_steps){
		diff = abs(this->evaluate_at(i) - g2.evaluate_at(i));
		if(diff<min_diff){
			min_diff = diff;
			min_diff_pos = i;
		}
	}

	return min_diff_pos;
}

gaussian_2d::gaussian_2d(float amp, float cent_x, float cent_y, float sig_u, float sig_v, float angle){
	amplitude = amp;
	center_x = cent_x;
	center_y = cent_y;
	sigma_u = sig_u;
	sigma_v = sig_v;
	phi = angle;
}

gaussian_2d::~gaussian_2d(){}

float gaussian_2d::evaluate_at(float x, float y){
	float cos_phi = cos(phi);
	float sin_phi = sin(phi);
	float sigma_u_2 = pow(sigma_u,2);
	float sigma_v_2 = pow(sigma_v,2);
	float dx = x-center_x;
	float dy = y-center_y;
	return amplitude* exp( -0.5 * (pow( dx*cos_phi + dy*sin_phi , 2)/sigma_u_2 + pow( dy*cos_phi - dx*sin_phi , 2)/sigma_v_2) );
}

//-------------------

rayleighian::rayleighian(float amp_val, float sigma_val)
{
	amp = amp_val;			//amp - can be initialted from histogram data using (amp = max_hist_val*sig*e(0.5))
	sig = sigma_val;		//sigma - can be initialted from histogram data using (sig = intensity_at_hist_max)
	sig2 = sig*sig;			//for speedup

	//...previously...
	//amp - can be initialted from histogram data using (amp = 0.5*Max_hist_val*M*e)
	//M - can be initialted from histogram data using (M = sqrt(2.0)*intensity_at_hist_max)
}

rayleighian::~rayleighian(){}

float rayleighian::evaluate_at(float x)
{
	return amp*x/(sig2)*exp( -(x*x)/(2*sig2) );
}

float rayleighian::get_variance()
{
	return 0.5*(4-pt_PI)*sig2;
}

float rayleighian::get_std()
{
	return sqrt(0.5*(4-pt_PI))*sig;
}
//-------------------


double gamma(double x)
{
	return vnl_gamma(x); //Uses 6 parameter Lanczos approximation as described by Toth (http://www.rskey.org/gamma.htm) Accurate to about one part in 3e-11. 
}

line2D::line2D()
{
	point = create_Vector2D(0,0);
	direction = create_Vector2D(1,1);
}

line2D::line2D(Vector2D pnt1, Vector2D pnt2)
{
	pt_error::error_if_false((pnt2[0]-pnt1[0])!=0 || (pnt2[1]-pnt1[1])!=0, "Direction must be non-zero vector", pt_error::debug);
	point = pnt1;
	direction = pnt2-pnt1;
}

line2D::line2D(float x1, float y1, float x2, float y2)
{
	pt_error::error_if_false((x2-x1)!=0 || (y2-y1)!=0, "Direction must be non-zero vector", pt_error::debug);
	point = create_Vector2D(x1,y1);
	direction = create_Vector2D(x2-x1, y2-y1);
}

Vector2D line2D::get_point()
{
	return point;
}

Vector2D line2D::get_direction()
{
	return direction;
}

void line2D::set_point(Vector2D pnt)
{
	point=pnt;
}

void line2D::set_point(float point_x, float point_y)
{
	point = create_Vector2D(point_x,point_y);
}

void line2D::set_direction(Vector2D dir)
{
	pt_error::error_if_false(dir[0]!=0 || dir[1]!=0, "Direction must be non-zero vector", pt_error::debug);
	direction=dir;
}

void line2D::set_direction(float dir_x, float dir_y)
{
	pt_error::error_if_false(dir_x!=0 || dir_y!=0, "Direction must be non-zero vector", pt_error::debug);
	direction = create_Vector2D(dir_x,dir_y);
}

bool line2D::is_point_left_of_line(Vector2D pnt)
{
	return (pnt[1]-point[1])*direction[0]<(pnt[0]-point[0])*direction[1];
}

bool line2D::is_point_right_of_line(Vector2D pnt)
{
	return (pnt[1]-point[1])*direction[0]>(pnt[0]-point[0])*direction[1];
}

bool line2D::is_point_left_of_line(float x, float y)
{
	return is_point_left_of_line(create_Vector2D(x,y));
}

bool line2D::is_point_right_of_line(float x, float y)
{
	return is_point_right_of_line(create_Vector2D(x,y));
}

line3D::line3D()
{
	point = create_Vector3D(0,0,0);
	direction = create_Vector3D(1,1,1);
}

line3D::line3D(Vector3D pnt, Vector3D dir)
{
	pt_error::error_if_false(dir[0]!=0 || dir[1]!=0 || dir[2]!=0, "line3D-Direction must be non-zero vector", pt_error::debug);
	point = pnt;
	direction = dir;
}

line3D::line3D(float point_x, float point_y, float point_z, float dir_x, float dir_y, float dir_z)
{
	pt_error::error_if_false(dir_x!=0 || dir_y!=0 || dir_z!=0, "line3D-Direction must be non-zero vector", pt_error::debug);
	point = create_Vector3D(point_x,point_y,point_z);
	direction = create_Vector3D(dir_x, dir_y, dir_z);
}

Vector3D line3D::get_point()
{
	return point;
}

Vector3D line3D::get_direction()
{
	return direction;
}

void line3D::set_point(Vector3D pnt)
{
	point=pnt;
}

void line3D::set_point(float point_x, float point_y, float point_z)
{
	point = create_Vector3D(point_x,point_y,point_z);
}

void line3D::set_direction(Vector3D dir)
{
	pt_error::error_if_false(dir[0]!=0 || dir[1]!=0 || dir[2]!=0, "Direction must be non-zero vector", pt_error::debug);
	direction=dir;
}

void line3D::set_direction(float dir_x, float dir_y, float dir_z)
{
	pt_error::error_if_false(dir_x!=0 || dir_y!=0 || dir_z!=0, "Direction must be non-zero vector", pt_error::debug);
	direction = create_Vector3D(dir_x,dir_y,dir_z);
}


void line3D::least_square_fit_line_to_points_in_3D(vector<Vector3D> points, int dir)
{
	this->set_point_to_center_of_gravity_from_points_in_3D(points);
	this->set_direction_to_point_cloud_variations_given_one_dir(dir, points);
}

std::ostream &operator<<(std::ostream &ut, const line3D &l)
{
	ut<<"line3D -point"<<l.point<<" -direction"<<l.direction;
	return ut;
}

void line3D::set_point_to_center_of_gravity_from_points_in_3D(vector<Vector3D> points)
{
	for(int p=0;p<points.size();p++){
		this->point += points[p];
	}
	this->point /= points.size();
	cout<<"set_point_to_center_of_gravity_from_points_in_3D -->"<<this->point<<endl;
}

void line3D::set_direction_to_point_cloud_variations_given_one_dir(int dir, vector<Vector3D> points)
{
	Vector3D R = create_Vector3D(0,0,0);
	float da=0;
	float daa=0;
	for(int p=0;p<points.size();p++){
		da = points[p][dir]-this->point[dir];
		R[0] += (points[p][0]-this->point[0])*da;
		R[1] += (points[p][1]-this->point[1])*da;
		R[2] += (points[p][2]-this->point[2])*da;
		daa += da*da;
	}
	R[0] = R[0]/daa;	//Rxy/Ryy for example...
	R[1] = R[1]/daa;	//Rxy/Ryy for example...
	R[2] = R[2]/daa;	//Rxy/Ryy for example...
	R[dir] = 1;			//Ryy/Ryy = 1;
	this->direction = R;
	cout<<"direction="<<this->direction<<endl;
}

plane3D::plane3D()
{
	point = create_Vector3D(0,0,0);
	normal = create_Vector3D(1,0,0);
	defined=false;

}

plane3D::plane3D(Vector3D pnt, Vector3D norm)
{
	pt_error::error_if_false(norm[0]!=0 || norm[1]!=0 || norm[2]!=0, "Normal must be non-zero vector", pt_error::debug);
	point = pnt;
	normal = norm;
	defined=true;
}

plane3D::plane3D(float point_x, float point_y, float point_z, float normal_x, float normal_y, float normal_z)
{
	pt_error::error_if_false(normal_x!=0 || normal_y!=0 || normal_z!=0, "Normal must be non-zero vector", pt_error::debug);
	point = create_Vector3D(point_x,point_y,point_z);
	normal = create_Vector3D(normal_x,normal_y,normal_z);
	defined=true;
}

plane3D::plane3D(Vector3D point1, Vector3D point2, Vector3D point3) // Construct plane from 3 points, not laying on a straight line
{
	point = point1;
	Vector3D dir1 = create_Vector3D(point2[0]-point1[0],point2[1]-point1[1],point2[2]-point1[2]);
	Vector3D dir2 = create_Vector3D(point3[0]-point1[0],point3[1]-point1[1],point3[2]-point1[2]);
	normal = CrossProduct (dir1, dir2);
	pt_error::error_if_false(normal[0]!=0 || normal[1]!=0 || normal[2]!=0, "Normal must be non-zero vector", pt_error::debug);
	defined=true;
}

Vector3D plane3D::get_point()
{
	return point;
}

Vector3D plane3D::get_normal()
{
	return normal;
}

bool plane3D::is_defined()
{
	return defined;
}

void plane3D::set_point(Vector3D pnt)
{
	point=pnt;
	defined=true;
}

void plane3D::set_point(float point_x, float point_y, float point_z)
{
	point=create_Vector3D(point_x,point_y,point_z);
	defined=true;
}

void plane3D::set_normal(Vector3D norm)
{
	pt_error::error_if_false(norm[0]!=0 || norm[1]!=0 || norm[2]!=0, "Normal must be non-zero vector", pt_error::debug);
	normal=norm;
	defined=true;
}

void plane3D::set_normal(float normal_x, float normal_y, float normal_z)
{
	pt_error::error_if_false(normal_x!=0 || normal_y!=0 || normal_z!=0, "Normal must be non-zero vector", pt_error::debug);
	normal = create_Vector3D(normal_x,normal_y,normal_z);
	defined=true;
}

void plane3D::invert()
{
	normal*=-1;
}

Vector3D plane3D::get_point_of_intersection(line3D line)
{
	return line.get_point()+((point-line.get_point())*normal)/(line.get_direction()*normal)*line.get_direction();
}

line3D plane3D::get_line_of_intersection(plane3D plane)
{
	Vector3D dir=CrossProduct(normal, plane.get_normal());
	float d1=-point*normal;
	float d2=-plane.get_point()*plane.get_normal();
	Vector3D pnt=CrossProduct((d2*normal-d1*plane.get_normal()), dir)/dir.GetSquaredNorm();
	line3D line=line3D(pnt,dir);
	return line;
}

line2D plane3D::get_projected_line(line3D line)
{
	cout<<"get_projected_line..."<<endl;
	cout<<this->get_point()<<" "<<this->get_normal()<<endl;

	Vector3D p1 = this->get_point_of_intersection(line);
	cout<<"p1= "<<p1<<endl;
	line3D tmp = line3D(p1+line.get_direction(),-this->get_normal());
	cout<<"tmp= "<<tmp.get_point()<<" "<<tmp.get_direction()<<endl;
	Vector3D p2 = this->get_point_of_intersection(tmp);
	cout<<"p2= "<<p2<<endl;
	line3D res2 = line3D(p1,p2-p1);
	cout<<"res2= "<<res2.get_point()<<" "<<res2.get_direction()<<endl;
	Vector3D a = res2.get_point();
	Vector3D b = res2.get_direction();
	line2D res = line2D(a[0],a[1],a[0]+b[0],a[1]+b[1]);
	cout<<"-->"<<res.get_point()<<" "<<res.get_direction()<<endl;
	return res;
}

std::ostream &operator<<(std::ostream &ut, const plane3D &p)
{
	ut<<"plane3D -point"<<p.point<<" -normal"<<p.normal;
	return ut;
}

ellipsoid3D::ellipsoid3D()
{
	_centrum = create_Vector3D(0,0,0);
	_radii = create_Vector3D(1,1,1);
	_azimuth = 0;
	_elevation = 0;
}

ellipsoid3D::ellipsoid3D(Vector3D centrum, Vector3D radii, float azimuth, float elevation)
{
	_centrum = centrum;
	_radii = radii;
	_azimuth = azimuth;
	_elevation = elevation;
}

ellipsoid3D::ellipsoid3D(Vector3D centrum, float radius_u, float radius_v, float radius_w, float azimuth, float elevation)
{
	_centrum = centrum;
	_radii = create_Vector3D(radius_u,radius_v,radius_w);
	_azimuth = azimuth;
	_elevation = elevation;
}

ellipsoid3D::ellipsoid3D(float centrum_x, float centrum_y, float centrum_z, float radius_u, float radius_v, float radius_w, float azimuth, float elevation)
{
	_centrum = create_Vector3D(centrum_x,centrum_y,centrum_z);
	_radii = create_Vector3D(radius_u,radius_v,radius_w);
	_azimuth = azimuth;
	_elevation = elevation;
}

Vector3D ellipsoid3D::get_centrum()
{
	return _centrum;
}
	
Vector3D ellipsoid3D::get_radii()
{
	return _radii;
}

float ellipsoid3D::get_azimuth()
{
	return _azimuth;
}

float ellipsoid3D::get_elevation()
{
	return _elevation;
}

void ellipsoid3D::set_centrum(Vector3D centrum)
{
	_centrum = centrum;
}

void ellipsoid3D::set_centrum(float centrum_x, float centrum_y, float centrum_z)
{
	_centrum = create_Vector3D(centrum_x, centrum_y, centrum_z);
}
	
void ellipsoid3D::set_radii(Vector3D radii)
{
	_radii = radii;
}
	
void ellipsoid3D::set_radii(float radius_u, float radius_v, float radius_w)
{
	_radii = create_Vector3D(radius_u, radius_v, radius_w);
}

void ellipsoid3D::set_azimuth(float azimuth)
{
	_azimuth = azimuth;
}

void ellipsoid3D::set_elevation(float elevation)
{
	_elevation = elevation;
}

bool ellipsoid3D::is_point_inside_ellipsoid(float x, float y, float z)
{
	float cos_a=cos(_azimuth);
	float sin_a=sin(_azimuth);
	float cos_e=cos(_elevation);
	float sin_e=sin(_elevation);
	float dx=x-_centrum[0];
	float dy=y-_centrum[1];
	float dz=z-_centrum[2];
	float du=(dx*cos_a+dy*sin_a)*cos_e + dz*sin_e;
	float dv=dy*cos_a-dx*sin_a;
	float dw=dz*cos_e-(dx*cos_a+dy*sin_a)*sin_e;
	return pow(du,2)/pow(_radii[0],2) + pow(dv,2)/pow(_radii[1],2) + pow(dw,2)/pow(_radii[2],2) < 1;
}
	
bool ellipsoid3D::is_point_inside_ellipsoid(Vector3D pnt)
{
	float cos_a=cos(_azimuth);
	float sin_a=sin(_azimuth);
	float cos_e=cos(_elevation);
	float sin_e=sin(_elevation);
	float dx=pnt[0]-_centrum[0];
	float dy=pnt[1]-_centrum[1];
	float dz=pnt[2]-_centrum[2];
	float du=(dx*cos_a+dy*sin_a)*cos_e + dz*sin_e;
	float dv=dy*cos_a-dx*sin_a;
	float dw=dz*cos_e-(dx*cos_a+dy*sin_a)*sin_e;
	return pow(du,2)/pow(_radii[0],2) + pow(dv,2)/pow(_radii[1],2) + pow(dw,2)/pow(_radii[2],2) < 1;
}

bool convex_last_three_points(vector<Vector2D>::iterator end, bool lower)
{
	line2D line=line2D(*(end-3), *(end-1));
	if (lower) 
		return line.is_point_right_of_line(*(end-2));
	else 
		return line.is_point_left_of_line(*(end-2));
}

vector<Vector2D> get_convex_hull_2D(vector<Vector2D> points)
{
	// GRAHAM SCAN:
	Vector2D left=points.front();
	points.erase(points.begin());
	Vector2D right=points.back();
	points.pop_back();
	
	line2D middle=line2D(left, right);
	Vector2D point;
	vector<Vector2D> hull, upper_hull, lower_hull, upper, lower;
	upper.push_back(right);
	lower.push_back(right);
	while (points.size()!=0) {
		point=points.back();
		points.pop_back();
		if (middle.is_point_left_of_line(point))
			upper.push_back(point);
		else if (middle.is_point_right_of_line(point))
			lower.push_back(point);
	}
	
	lower_hull.push_back(left);
	while (lower.size()!=0) {
		lower_hull.push_back(lower.back());
		lower.pop_back();
		while (lower_hull.size()>=3 && !convex_last_three_points(lower_hull.end(), true))
			lower_hull.erase(lower_hull.end()-2);
	}

	upper_hull.push_back(left);
	while (upper.size()!=0) {
		upper_hull.push_back(upper.back());
		upper.pop_back();
		while (upper_hull.size()>=3 && !convex_last_three_points(upper_hull.end(), false))
			upper_hull.erase(upper_hull.end()-2);
	}
	
	vector<Vector2D>::iterator i;
	for (i=lower_hull.begin(); i!=lower_hull.end()-1; i++)
		hull.push_back(*i);
	for (i=upper_hull.end()-1; i!=upper_hull.begin(); i--)
		hull.push_back(*i);

	return hull;
}

void pt_spline1D(float x[],float y[],int n,float yp1,float ypn,float y2[])
{
	int i,k;
	float p,qn,sig,un,*u;

	u = new float[n+2];

	if (yp1 > 0.99e30)			//The lower boundary condition is set either to be "natural" y2=0...
		y2[1] = u[1] = 0.0;		
	else {						//...or else to have a specific first derivative.
		y2[1] = -0.5;			
		u[1] = (3.0/(x[2]-x[1]))*((y[2]-y[1])/(x[2]-x[1])-yp1);
	}

	for(i=2;i<=n;i++) {
		sig = (x[i]-x[i-1])/(x[i+1]-x[i-1]);
		p = sig*y2[i-1]+2.0;
		y2[i] = (sig-1.0)/p;
		u[i] = (y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
		u[i] = (6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
	}
	if (ypn > 0.99e30)
		qn = un = 0.0;
	else {
		qn=0.5;
		un=(3.0/(x[n]-x[n-1]))*(ypn-(y[n]-y[n-1])/(x[n]-x[n-1]));
	}

	y2[n] = (un-qn*u[n-1])/(qn*y2[n-1]+1.0);
	for (k=n-1;k>=1;k--)
		y2[k]=y2[k]*y2[k+1]+u[k];

	delete u;
}


float pt_splint1D(float xa[],float ya[],float y2a[],int n,float x)
{
	int klo,khi,k;
	float h,b,a;

	klo=1;
	khi=n;
	while (khi-klo > 1) {
		k=(khi+klo) >> 1;
		if (xa[k] > x) khi=k;
		else klo=k;
	}
	h=xa[khi]-xa[klo];
	if ( h == 0.0) 
		std::cout<<"Bad xa input to routine splint"<<std::endl;
	a = (xa[khi]-x)/h;
	b = (x-xa[klo])/h;

	return a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)/6.0;
}

/*
Matrix3D matrix_generator::get_rot_x_matrix_3D(float fi)			//fi in radians
{
    Matrix3D r;
    r[0][0] = 1;	r[1][0] = 0; 		r[2][0] = 0;
    r[0][1] = 0;	r[1][1] = cos(fi); 	r[2][1] = -sin(fi);
    r[0][2] = 0;	r[1][2] = sin(fi); 	r[2][2] = cos(fi);
    return r;
}

Matrix3D matrix_generator::get_rot_y_matrix_3D(float fi)			//fi in radians
{
    Matrix3D r;
    r[0][0] = cos(fi);	r[1][0] = 0; 		r[2][0] = sin(fi);
    r[0][1] = 0;		r[1][1] = 1;		r[2][1] = 0;
    r[0][2] = -sin(fi);	r[1][2] = 0;		r[2][2] = cos(fi);
    return r;
}

Matrix3D matrix_generator::get_rot_z_matrix_3D(float fi)			//fi in radians
{
    Matrix3D r;
    r[0][0] = cos(fi);	r[1][0] = -sin(fi);	r[2][0] = 0;
    r[0][1] = sin(fi);	r[1][1] = cos(fi);	r[2][1] = 0;
    r[0][2] = 0;		r[1][2] = 0;		r[2][2] = 1;
    return r;
}

// Note that order of the angles are z, y and x!
Matrix3D matrix_generator::get_rot_matrix_3D(int fi_z, int fi_y, int fi_x)	//fi_z/y/x in radians
{
    return get_rot_z_matrix_3D(float(fi_z)*pt_PI/180.0)*get_rot_y_matrix_3D(float(fi_y)*pt_PI/180.0)*get_rot_x_matrix_3D(float(fi_x)*pt_PI/180.0);
}

// Note that order of the angles are z, y and x!
Matrix3D matrix_generator::get_rot_matrix_3D(float fi_z, float fi_y, float fi_x)	//fi_z/y/x in radians
{
    return get_rot_z_matrix_3D(fi_z)*get_rot_y_matrix_3D(fi_y)*get_rot_x_matrix_3D(fi_x);
}
*/

Matrix3D create_rot_x_matrix_3D(float fi_rad)
{
    Matrix3D r;
    r[0][0] = 1;	r[1][0] = 0; 			r[2][0] = 0;
    r[0][1] = 0;	r[1][1] = cos(fi_rad); 	r[2][1] = -sin(fi_rad);
    r[0][2] = 0;	r[1][2] = sin(fi_rad); 	r[2][2] = cos(fi_rad);
    return r;
}

Matrix3D create_rot_y_matrix_3D(float fi_rad)
{
    Matrix3D r;
    r[0][0] = cos(fi_rad);	r[1][0] = 0; 		r[2][0] = sin(fi_rad);
    r[0][1] = 0;			r[1][1] = 1;		r[2][1] = 0;
    r[0][2] = -sin(fi_rad);	r[1][2] = 0;		r[2][2] = cos(fi_rad);
    return r;
}

Matrix3D create_rot_z_matrix_3D(float fi_rad)
{
    Matrix3D r;
	r[0][0] = cos(fi_rad);	r[1][0] = -sin(fi_rad);	r[2][0] = 0;
    r[0][1] = sin(fi_rad);	r[1][1] = cos(fi_rad);	r[2][1] = 0;
    r[0][2] = 0;			r[1][2] = 0;			r[2][2] = 1;
     return r;
}

Matrix3D create_rot_matrix_3D(float fi_x_rad, float fi_y_rad, float fi_z_rad)	//fi_z/y/x in radians
{
	//matrixes shall be mlutiplied as m_z*m_y*m_x... // After many mistakes... The arguments are given as x/y/z - rotation
    return create_rot_z_matrix_3D(fi_z_rad)*create_rot_y_matrix_3D(fi_y_rad)*create_rot_x_matrix_3D(fi_x_rad);
}


// Tricubic interpolation using method described in:
// F. Lekien, J.E. Marsden
// Tricubic Interpolation in Three Dimensions
// International Journal for Numerical Methods in Engineering, 63 (3), 455-471, 2005
// ...used in the function: image_scalar.interpolate_tricubic_3D...

const int A_tricubic[64][64] = {
{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{-3, 3, 0, 0, 0, 0, 0, 0,-2,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 2,-2, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{-3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0,-3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 9,-9,-9, 9, 0, 0, 0, 0, 6, 3,-6,-3, 0, 0, 0, 0, 6,-6, 3,-3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{-6, 6, 6,-6, 0, 0, 0, 0,-3,-3, 3, 3, 0, 0, 0, 0,-4, 4,-2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2,-2,-1,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 2, 0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 2, 0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{-6, 6, 6,-6, 0, 0, 0, 0,-4,-2, 4, 2, 0, 0, 0, 0,-3, 3,-3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2,-1,-2,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 4,-4,-4, 4, 0, 0, 0, 0, 2, 2,-2,-2, 0, 0, 0, 0, 2,-2, 2,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 3, 0, 0, 0, 0, 0, 0,-2,-1, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,-2, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0,-1, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9,-9,-9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 3,-6,-3, 0, 0, 0, 0, 6,-6, 3,-3, 0, 0, 0, 0, 4, 2, 2, 1, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-6, 6, 6,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3,-3, 3, 3, 0, 0, 0, 0,-4, 4,-2, 2, 0, 0, 0, 0,-2,-2,-1,-1, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-6, 6, 6,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-4,-2, 4, 2, 0, 0, 0, 0,-3, 3,-3, 3, 0, 0, 0, 0,-2,-1,-2,-1, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,-4,-4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2,-2,-2, 0, 0, 0, 0, 2,-2, 2,-2, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
{-3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0,-3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 9,-9, 0, 0,-9, 9, 0, 0, 6, 3, 0, 0,-6,-3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,-6, 0, 0, 3,-3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{-6, 6, 0, 0, 6,-6, 0, 0,-3,-3, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-4, 4, 0, 0,-2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2,-2, 0, 0,-1,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0, 0, 0,-1, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9,-9, 0, 0,-9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 3, 0, 0,-6,-3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,-6, 0, 0, 3,-3, 0, 0, 4, 2, 0, 0, 2, 1, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-6, 6, 0, 0, 6,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3,-3, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-4, 4, 0, 0,-2, 2, 0, 0,-2,-2, 0, 0,-1,-1, 0, 0},
{ 9, 0,-9, 0,-9, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 3, 0,-6, 0,-3, 0, 6, 0,-6, 0, 3, 0,-3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 2, 0, 2, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 9, 0,-9, 0,-9, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 3, 0,-6, 0,-3, 0, 6, 0,-6, 0, 3, 0,-3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 2, 0, 2, 0, 1, 0},
{-27,27,27,-27,27,-27,-27,27,-18,-9,18, 9,18, 9,-18,-9,-18,18,-9, 9,18,-18, 9,-9,-18,18,18,-18,-9, 9, 9,-9,-12,-6,-6,-3,12, 6, 6, 3,-12,-6,12, 6,-6,-3, 6, 3,-12,12,-6, 6,-6, 6,-3, 3,-8,-4,-4,-2,-4,-2,-2,-1},
{18,-18,-18,18,-18,18,18,-18, 9, 9,-9,-9,-9,-9, 9, 9,12,-12, 6,-6,-12,12,-6, 6,12,-12,-12,12, 6,-6,-6, 6, 6, 6, 3, 3,-6,-6,-3,-3, 6, 6,-6,-6, 3, 3,-3,-3, 8,-8, 4,-4, 4,-4, 2,-2, 4, 4, 2, 2, 2, 2, 1, 1},
{-6, 0, 6, 0, 6, 0,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 0,-3, 0, 3, 0, 3, 0,-4, 0, 4, 0,-2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0,-2, 0,-1, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0,-6, 0, 6, 0, 6, 0,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 0,-3, 0, 3, 0, 3, 0,-4, 0, 4, 0,-2, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0,-2, 0,-1, 0,-1, 0},
{18,-18,-18,18,-18,18,18,-18,12, 6,-12,-6,-12,-6,12, 6, 9,-9, 9,-9,-9, 9,-9, 9,12,-12,-12,12, 6,-6,-6, 6, 6, 3, 6, 3,-6,-3,-6,-3, 8, 4,-8,-4, 4, 2,-4,-2, 6,-6, 6,-6, 3,-3, 3,-3, 4, 2, 4, 2, 2, 1, 2, 1},
{-12,12,12,-12,12,-12,-12,12,-6,-6, 6, 6, 6, 6,-6,-6,-6, 6,-6, 6, 6,-6, 6,-6,-8, 8, 8,-8,-4, 4, 4,-4,-3,-3,-3,-3, 3, 3, 3, 3,-4,-4, 4, 4,-2,-2, 2, 2,-4, 4,-4, 4,-2, 2,-2, 2,-2,-2,-2,-2,-1,-1,-1,-1},
{ 2, 0, 0, 0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{-6, 6, 0, 0, 6,-6, 0, 0,-4,-2, 0, 0, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 3, 0, 0,-3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2,-1, 0, 0,-2,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 4,-4, 0, 0,-4, 4, 0, 0, 2, 2, 0, 0,-2,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,-2, 0, 0, 2,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-6, 6, 0, 0, 6,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-4,-2, 0, 0, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-3, 3, 0, 0,-3, 3, 0, 0,-2,-1, 0, 0,-2,-1, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,-4, 0, 0,-4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0,-2,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,-2, 0, 0, 2,-2, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0},
{-6, 0, 6, 0, 6, 0,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0,-4, 0,-2, 0, 4, 0, 2, 0,-3, 0, 3, 0,-3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0,-1, 0,-2, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0,-6, 0, 6, 0, 6, 0,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-4, 0,-2, 0, 4, 0, 2, 0,-3, 0, 3, 0,-3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,-2, 0,-1, 0,-2, 0,-1, 0},
{18,-18,-18,18,-18,18,18,-18,12, 6,-12,-6,-12,-6,12, 6,12,-12, 6,-6,-12,12,-6, 6, 9,-9,-9, 9, 9,-9,-9, 9, 8, 4, 4, 2,-8,-4,-4,-2, 6, 3,-6,-3, 6, 3,-6,-3, 6,-6, 3,-3, 6,-6, 3,-3, 4, 2, 2, 1, 4, 2, 2, 1},
{-12,12,12,-12,12,-12,-12,12,-6,-6, 6, 6, 6, 6,-6,-6,-8, 8,-4, 4, 8,-8, 4,-4,-6, 6, 6,-6,-6, 6, 6,-6,-4,-4,-2,-2, 4, 4, 2, 2,-3,-3, 3, 3,-3,-3, 3, 3,-4, 4,-2, 2,-4, 4,-2, 2,-2,-2,-1,-1,-2,-2,-1,-1},
{ 4, 0,-4, 0,-4, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0,-2, 0,-2, 0, 2, 0,-2, 0, 2, 0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
{ 0, 0, 0, 0, 0, 0, 0, 0, 4, 0,-4, 0,-4, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0,-2, 0,-2, 0, 2, 0,-2, 0, 2, 0,-2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0},
{-12,12,12,-12,12,-12,-12,12,-8,-4, 8, 4, 8, 4,-8,-4,-6, 6,-6, 6, 6,-6, 6,-6,-6, 6, 6,-6,-6, 6, 6,-6,-4,-2,-4,-2, 4, 2, 4, 2,-4,-2, 4, 2,-4,-2, 4, 2,-3, 3,-3, 3,-3, 3,-3, 3,-2,-1,-2,-1,-2,-1,-2,-1},
{ 8,-8,-8, 8,-8, 8, 8,-8, 4, 4,-4,-4,-4,-4, 4, 4, 4,-4, 4,-4,-4, 4,-4, 4, 4,-4,-4, 4, 4,-4,-4, 4, 2, 2, 2, 2,-2,-2,-2,-2, 2, 2,-2,-2, 2, 2,-2,-2, 2,-2, 2,-2, 2,-2, 2,-2, 1, 1, 1, 1, 1, 1, 1, 1}};



float pt_LoG(float x, float sigma)
{
	float x2 = x*x;
	float sigma2 = sigma*sigma;
	return -(x2-sigma2)/(sigma2*sigma2)*exp( -(x2/(2*sigma2)) );
}


void print_datatype_numerical_limits()
{
	std::cout<<"Data Type: \t min \t\t max \t\t digits"<<std::endl;
	std::cout<<"---------------------------------------"<<std::endl;
	std::cout<<"double:   \t";		print_type_limits<double>();
//	std::cout<<"long double: \t";	print_type_limits<long double>();
	std::cout<<"float: \t";			print_type_limits<float>();
	std::cout<<"short: \t";			print_type_limits<short>();
	std::cout<<"unsigned short: \t";print_type_limits<unsigned short>();
	std::cout<<"int: \t";			print_type_limits<int>();
	std::cout<<"unsigned int: \t";	print_type_limits<unsigned int>();
	std::cout<<"long: \t";			print_type_limits<long>();
	std::cout<<"unsigned long: \t";	print_type_limits<unsigned long>();
	std::cout<<"char: \t";			print_type_limits<char>();
	std::cout<<"unsigned char: \t";	print_type_limits<unsigned char>();
	std::cout<<"signed char: \t";	print_type_limits<signed char>();
	std::cout<<"bool: \t";			print_type_limits<bool>();
}



double get_random_number_in_span(double min, double max)
{
	return min + (max-min)*(double(rand())/double(RAND_MAX));
}

Vector3D create_Vector3D(float x, float y, float z)
{
	Vector3D v;
	v[0]=x;
	v[1]=y;
	v[2]=z;
	return v;
}

Vector3D create_Vector3D(vnl_vector<float> &v)
{
	return create_Vector3D(v(0),v(1),v(2));
}


Vector3Dint create_Vector3Dint( int x, int y, int z )
{
	Vector3Dint v;
	v[0] = x;
	v[1] = y;
	v[2] = z;
	return v;
}

Vector3Dint create_Vector3Dint(Vector3D v)
{
	Vector3Dint v2;
	v2[0] = v[0];
	v2[1] = v[1];
	v2[2] = v[2];
	return v2;
}

Vector3Di create_Vector3Di(int x, int y, int z)
{
	Vector3Di v;
	v.data[0]=x;
	v.data[1]=y;
	v.data[2]=z;
	return v;
}

Vector2D create_Vector2D(float x, float y)
{
	Vector2D v;
	v[0] = x;
	v[1] = y;
	return v;
}

Vector2Dint create_Vector2Dint(int x, int y)
{
	Vector2Dint v;
	v[0] = x;
	v[1] = y;
	return v;
}
void save_Vector3D_to_file(Vector3D v,string filepath)
{
	ofstream myfile;
	myfile.open(filepath.c_str());
	myfile<<v[0]<<";"<<v[1]<<";"<<v[2]<<";";
	myfile.close();
}

void save_vector_of_Vector3D_to_file(vector<Vector3D> v,string filepath)
{
	ofstream myfile;
	myfile.open(filepath.c_str());
	for(int i=0;i<v.size();i++){
		myfile<<v[i][0]<<";"<<v[i][1]<<";"<<v[i][2]<<";\n";
	}
	myfile.close();
}

Vector3D string2Vector3D(string s)
{
	Vector3D v;
	string word;

	int ind1=0;
	int ind2=0;
	vector<string> row;

	ind1=0;
	ind2=0;
	row = vector<string>();

	while(ind2>=0){
		ind2 = s.find_first_of(";",ind1);
		word = s.substr(ind1,ind2-ind1);
//		cout<<ind1<<" "<<ind2<<"  "<<word.c_str()<<"  ("<<s.c_str()<<")"<<endl;
		ind1 = ind2+1;
		if(ind2>=0){			 //the row is set to end with an ";"
			row.push_back(word);
//			cout<<"word="<<word<<endl;
		}
	}
//	cout<<"row[0].c_str()="<<row[0].c_str()<<endl;
//	cout<<"row[1].c_str()="<<row[1].c_str()<<endl;
//	cout<<"row[2].c_str()="<<row[2].c_str()<<endl;
	v[0] = atof(row[0].c_str());
	v[1] = atof(row[1].c_str());
	v[2] = atof(row[2].c_str());

	return v;
}

Vector3D load_Vector3D_from_file(string filepath)
{
	Vector3D v;
	ifstream myfile(filepath.c_str());

	if(myfile.is_open()){
		char buffer[10000];
		string s;

		while(!myfile.eof()){
			myfile.getline(buffer,10000);
			s=string(buffer);
			v = string2Vector3D(s);
		}
		myfile.close();
	}

	return v;
}

vector<Vector3D> load_vector_of_Vector3D_from_file(string filepath)
{
	Vector3D v;
	vector<Vector3D> v2;
	ifstream myfile(filepath.c_str());

	if(myfile.is_open()){
		char buffer[10000];
		string s;

		while(!myfile.eof()){
			myfile.getline(buffer,10000);
			s=string(buffer);
			if(s != ""){
				v = string2Vector3D(s);
				v2.push_back(v);
			}
		}
		myfile.close();
	}

	return v2;
}

bool is_defined(float f)
{
	return (f>=0||f<0)? true:false;
}

bool is_defined(Vector3D v)
{
	return ( is_defined(v[0])&&is_defined(v[1])&&is_defined(v[2]) )?true:false;
}

int get_component_from_dir(int x, int y, int z, int dir)
{
	if(dir==0){
		return x;
	}else if(dir==1){
		return y;
	}
	return z;
}

float get_component_from_dir(float x, float y, float z, int dir)
{
	if(dir==0){
		return x;
	}else if(dir==1){
		return y;
	}
	return z;
}

float get_component_from_dir(Vector3D &v, int dir)
{
	if(dir==0){
		return v[0];
	}else if(dir==1){
		return v[1];
	}
	return v[2];
}

int get_component_from_dim_and_dir(int x, int y, int z, int dim, int dir)
{
	//(dim+direction+1)%IMAGEDIM
	return get_component_from_dir(x,y,z,(dim+dir+1)%3);
}

float get_component_from_dim_and_dir(float x, float y, float z, int dim, int dir)
{
	return get_component_from_dir(x,y,z,(dim+dir+1)%3);
}

float get_component_from_dim_and_dir(Vector3D &v, int dim, int dir)
{
	return get_component_from_dir(v[0],v[1],v[2],(dim+dir+1)%3);
}



Vector3D get_mean_Vector3D(vector<Vector3D> v)
{
	Vector3D mean = create_Vector3D(0,0,0);
	for(int i=0;i<v.size();i++){
		mean += v[i];
	}
	mean = mean/float(v.size());
	cout<<"mean[0]="<<mean[0]<<endl;
	cout<<"mean[1]="<<mean[1]<<endl;
	cout<<"mean[2]="<<mean[2]<<endl;
	return mean;
}

Vector3Dint get_mean_Vector3Dint(vector<Vector3D> v)
{
	Vector3D mean = get_mean_Vector3D(v);
	Vector3Dint mean_int;
	mean_int[0] = int(mean[0]);
	mean_int[1] = int(mean[1]);
	mean_int[2] = int(mean[2]);
	return mean_int;
}

string Vector3D2str(Vector3D v)
{
	return "("+float2str(v[0])+","+float2str(v[1])+","+float2str(v[2])+")";
}

string bool2str(bool b)
{
	return b ? "true":"false";
}



Matrix3D outer_product(const Vector3D a, const Vector3D b)
{
	Matrix3D m;
	
	for ( int i  = 0; i < 3; i++ )
	{
		for ( int j = 0; j < 3; j++ )
			{ m[i][j] = a[i] * b[j]; }
	}
	return m;
}

void copy_columns(Matrix3D &m_to, Matrix3D &m_from, int to_col, int from_col, bool negate)
{
	if(negate){
		for(int i=0;i<=2;i++){
			m_to[i][to_col] = - m_from[i][from_col];
		}
	}else{
		for(int i=0;i<=2;i++){
			m_to[i][to_col] = m_from[i][from_col];
		}
	}
}


string Matrix3D2str(Matrix3D m)
{
//	return float2str(m[0][0])+" "+float2str(m[0][1])+" "+float2str(m[0][2])+" "+float2str(m[1][0])+" "+float2str(m[1][1])+" "+float2str(m[1][2])+" "+ float2str(m[2][0])+" "+float2str(m[2][1])+" "+float2str(m[2][2]);
	return Matrix3Drow2str(m,0)+"\n"+Matrix3Drow2str(m,1)+"\n"+Matrix3Drow2str(m,2);
}

string Matrix3Drow2str(Matrix3D m, int row)
{
	return float2str(m[row][0])+" "+float2str(m[row][1])+" "+float2str(m[row][2]);
}


unsigned int get_factorial(unsigned int i)
{
	if(i<=1){
		return 1;
	}
	else{
		return i*get_factorial(i-1);
	}
}

unsigned int get_permutations(unsigned int n, unsigned int r)
{
	return get_factorial(n)/(get_factorial(r)*get_factorial(n-r));
}

unsigned int get_smallest_power_above(unsigned int this_val, unsigned int power_base)
{
	unsigned int res=0;
	for(unsigned int i=0;i<std::numeric_limits<unsigned int>::max();i++){
		res = pow(float(power_base),float(i));
		if(res>=this_val){
			return res;
		}
	}
	return 0;
}

float magnitude(const Vector3D v)
{
	return sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}

float magnitude(const Vector3Dint v)
{
	return sqrt(float(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]));
}

Vector3D mean(const std::vector<Vector3D> & x)
{
	Vector3D sum = create_Vector3D(0.0, 0.0, 0.0);
	for ( std::vector<Vector3D>::const_iterator itr = x.begin(); itr != x.end(); itr++ )
		{ sum += *itr; }
	sum /= x.size();
	return sum;
}

Matrix3D var(const std::vector<Vector3D> & x)
{
	const Vector3D xmean = mean(x);

	Matrix3D sx;
	sx.Fill(0.0);
	for ( std::vector<Vector3D>::const_iterator itr = x.begin(); itr != x.end(); itr++ )
		{ sx += outer_product(*itr - xmean, *itr - xmean); }

	sx /= (x.size() - 1);
		
	return sx;
}

// unbiased pooled covariance matrix estimate
Matrix3D pooled_cov(const std::vector<Vector3D> & x, const std::vector<Vector3D> & y)
{
	Matrix3D sx = var(x);
	sx *= (x.size() - 1);

	Matrix3D sy = var(y);
	sy *= (y.size() - 1);
	
	Matrix3D s = sx + sy;
	s /= (x.size() + y.size() - 2);
	
	return s;
}

float tsquare(const std::vector<Vector3D> & x, const std::vector<Vector3D> & y)
{
	const float nx = x.size();				// use float to avoid "int/int = int" later
	const float ny = y.size();
	const Vector3D xmean = mean(x);
	const Vector3D ymean = mean(y);
	const Matrix3D s = pooled_cov(x, y);
	const Matrix3D s_inv = static_cast<Matrix3D>(s.GetInverse());

	return ((nx * ny) / (nx + ny)) * ((xmean - ymean) * (s_inv * (xmean - ymean)));
}


void subtract_from_all(std::vector<Vector3D> &x, Vector3D value)
{
	for(int i=0; i<x.size(); i++){
		x[i] = x[i] - value;
	}
}

Matrix3D cov(std::vector<Vector3D> x, std::vector<Vector3D> y)
{
	Vector3D mx = mean(x);
	Vector3D my = mean(y);
	subtract_from_all(x,mx);
	subtract_from_all(y,my);

	return cov2(x,y);
}

Matrix3D cov2(std::vector<Vector3D> x, std::vector<Vector3D> y){ //with scaled vectors (where mean is subtracted)
	Matrix3D m;
	for(int r=0;r<3;r++){
		for(int c=0;c<3;c++){
			for(int i=0;i<x.size();i++){
				m[r][c] += x[i][r]*y[i][c];
			}
		}
	}
	cout<<"m"<<m<<endl;
	return m;
}


double invF(const double p, const double a, const double b)
{ 
	// Return inverse cumulative distribution function. 
	if ( p <= 0.0 || p >= 1.0 )
		{ pt_error::error("invF(): argument out of range", pt_error::serious); }
		
	double x = invIncompleteBeta(p, 0.5 * a, 0.5 * b); 
	return b * x / (a * (1.0 - x));
}

double invIncompleteBeta(const double p, const double a, const double b)
{
	double x;
	double i = 0;
	double j = 1;
	double precision = 1.0e-10;		// converge until 10 decimals

	while ( (j - i) > precision )
	{
		x = (i + j) / 2;
		
		if ( incompleteBeta(a, b, x) > p )
			{ j = x; }
		else
			{ i = x; }
	}
	return x;
}

// Lanczos approximation is used to compute the Gamma function.
// The logarithm of the Gamma function is computed since the
// Gamma function will overflow at quite small values.
double logGamma(double x)
{
	if ( x <= 0 )
		{ pt_error::error("gammaln(): value out of range", pt_error::serious); }

	const double c[7] = { 1.000000000190015, 76.18009172947146, -86.50532032941677,
		24.01409824083091, -1.231739572450155, 1.208650973866179e-3, -5.395239384953e-6 };

	double sum = 0.0;
	for ( int i = 1; i <= 6; i++ )
		{ sum += c[i] / (x + i); }

	return log((sqrt(2 * pt_PI) / x) * (c[0] + sum)) + (x + 0.5) * log(x + 5.5) - (x + 5.5);
}


double incompleteBeta(const double a, const double b, const double x)
{ 
	if ( a <= 0.0 || b <= 0.0 )
		{ pt_error::error("incompleteBeta(): argument out of range", pt_error::serious); }
	if ( x < 0.0 || x > 1.0 ) 
		{ pt_error::error("incompleteBeta(): argument out of range", pt_error::serious); }

	if ( x == 0.0 || x == 1.0 )
		{ return x; }

	double result = std::exp(logGamma(a + b) - logGamma(a) - logGamma(b) + a * std::log(x) + b * std::log(1.0 - x));
	
	if ( x < ((a + 1.0) / (a + b + 2.0)) )
		{ return result * incompleteBetaCF(a, b, x) / a; }
	else
		{ return 1.0 - result * incompleteBetaCF(b, a, 1.0 - x) / b; }
}

double incompleteBetaCF(const double a, const double b, const double x)
{
	const double accuracy = std::numeric_limits<double>::epsilon();
	const double qlepsilon = std::numeric_limits<double>::min() / accuracy;

	double aa, del;
	double qab = a + b;
	double qap = a + 1.0;
	double qam = a - 1.0;
	double c = 1.0;
	double d = 1.0 - qab * x / qap;
	
	if ( std::fabs(d) < qlepsilon )
		{ d = qlepsilon; }

	d = 1.0 / d;
	double result = d;

	int j;
	for ( int i = 1; i <= 10000; i++ )
	{
		j = 2 * i;
		aa = i * (b - i) * x / ((qam + j) * (a + j));
		d = 1.0 + aa * d;
		
		if ( std::fabs(d) < qlepsilon )
			{ d = qlepsilon; }
			
		c	= 1.0 + aa / c;
		
		if ( std::fabs(c) < qlepsilon )
			{ c = qlepsilon; }
			
		d = 1.0 / d;
		result *= d * c;
		aa = -(a + i) * (qab + i) * x / ((a + j) * (qap + j));
		d = 1.0 + aa * d;
		
		if ( std::fabs(d) < qlepsilon )
			{ d = qlepsilon; }
			
		c = 1.0 + aa / c;
		
		if ( std::fabs(c) < qlepsilon )
			{ c = qlepsilon; }
			
		d = 1.0 / d;
		del = d * c;
		result *= del;
		
		if ( std::fabs(del - 1.0) < accuracy )
			{ break; }
	}
	return result;
}

double regularizedGamma(const unsigned int N, const double x) {
	double Q = 0.0;
	for (int k=0; k<N; k++)
		Q +=pow(x,k)/get_factorial(k);
	Q *= exp(-x);
	return Q;
}

bool fit_points(const std::vector<Vector3D> & fixed, std::vector<Vector3D> & moving, 
	const std::map<int,int> & fixedToMoving)
{
	if ( fixedToMoving.size() < 3 )
	{
		pt_error::error("fit_points: number of points used for matching is less than 3",
			pt_error::warning);
		return false;
	}

	typedef unsigned char PixelType;
	const unsigned int Dimension = 3;

	typedef itk::Image<PixelType, Dimension> FixedImageType;
	typedef itk::Image<PixelType, Dimension> MovingImageType;
	
	// Set the transform type
	typedef itk::VersorRigid3DTransform<double> TransformType;
	TransformType::Pointer transform = TransformType::New();
	typedef itk::LandmarkBasedTransformInitializer<TransformType, FixedImageType, MovingImageType> 
		TransformInitializerType;
	TransformInitializerType::Pointer initializer = TransformInitializerType::New();

	TransformInitializerType::LandmarkPointContainer fixedLandmarks;
	TransformInitializerType::LandmarkPointContainer movingLandmarks;

	TransformInitializerType::LandmarkPointType fixedPoint;
	TransformInitializerType::LandmarkPointType movingPoint;


	for ( std::map<int,int>::const_iterator itr = fixedToMoving.begin(); 
		itr != fixedToMoving.end(); itr++ )
	{
		fixedPoint[0] = fixed[itr->first][0];
		fixedPoint[1] = fixed[itr->first][1];
		fixedPoint[2] = fixed[itr->first][2];
		fixedLandmarks.push_back(fixedPoint);
		
		movingPoint[0] = moving[itr->second][0];
		movingPoint[1] = moving[itr->second][1];
		movingPoint[2] = moving[itr->second][2];
		movingLandmarks.push_back(movingPoint);
	}
	
	initializer->SetFixedLandmarks(fixedLandmarks);
	initializer->SetMovingLandmarks(movingLandmarks);
	initializer->SetTransform(transform);
	initializer->InitializeTransform();

	TransformType::Pointer invTransform = TransformType::New();
	if ( !transform->GetInverse(invTransform) )
	{
		pt_error::error("fit_points: cannot compute inverse transformation",
			pt_error::warning);		
		return false;
	}
		
	// Transform the moving landmarks
	TransformInitializerType::LandmarkPointType transformedPoint;
	for ( std::vector<Vector3D>::iterator mitr = moving.begin(); mitr != moving.end(); mitr++ )
	{
		// "convert" from Vector3D to LandmarkPointType
		movingPoint[0] = (*mitr)[0];
		movingPoint[1] = (*mitr)[1];
		movingPoint[2] = (*mitr)[2];
		
		transformedPoint = invTransform->TransformPoint(movingPoint);

		(*mitr)[0] = transformedPoint[0];
		(*mitr)[1] = transformedPoint[1];
		(*mitr)[2] = transformedPoint[2];
	}
	
	return true;
}



float ttest_indep_2sample_equal_variance(vector<float> &group_a, vector<float> &group_b){
	float n_a = group_a.size();
	float n_b = group_b.size();
	float mean_a = 0;
	float mean_b = 0;
	float tmp = 0;
	float s_a = 0;
	float s_b = 0;

	//------- Means -----------
	for(int i=0;i<n_a;i++){
		mean_a += group_a[i];
	}
	mean_a /= n_a;


	for(int i=0;i<n_b;i++){
		mean_b += group_b[i];
	}
	mean_b /= n_b;

	//------- Variances (s) -----------
	for(int i=0;i<n_a;i++){
		tmp = group_a[i]-mean_a;
		s_a += tmp*tmp;
	}
	s_a /= (n_a-1); //the variance is estimated from the sample

	for(int i=0;i<n_b;i++){
		tmp = group_b[i]-mean_b;
		s_b += tmp*tmp;
	}
	s_b /= (n_b-1); //the variance is estimated from the sample


	// float S_p = ( (n_a-1)*s_a+(n_b-1)*s_b )/( n_a+n_b-2 ); //pooled variances

	float t = (mean_a-mean_b)/( sqrt(s_a/n_a+s_b/n_b));
	//cout<<n_a<<endl;
	//cout<<n_b<<endl;
	//cout<<mean_a<<endl;
	//cout<<mean_b<<endl;
	//cout<<s_a<<endl;
	//cout<<s_b<<endl;
	//cout<<S_p<<endl;
	//cout << "t: " << t << endl;
	return t;
}
int fit_line(vector<float> &x, vector<float> &y, float &slope, float &intercept){
	float corr=0;
	return fit_line(x, y, slope, intercept, corr);
}

int fit_line(vector<float> &x, vector<float> &y, float &slope, float &intercept, float &correlation){
	//For the line of best fit y=k*x+m:
	//k = (n *Sxy - Sx * Sy) / (n * Sxx - Sx * Sx)
	//m = (Sy - k * Sx) / n

	if( x.size() != y.size() ){
		return 0;
	}

	float n=x.size();		//number of points
	float Sx=0;				//the SUM of all the X coordinates
	float Sy=0;				//the SUM of all the Y coordinates
	float Sxy=0;			//the SUM of all (x * y)
	float Sxx=0;			//the SUM of all (x * x)
	float Syy=0;			//the SUM of all (y * y)

	for(int i=0;i<n;i++){
		Sx += x[i];
		Sy += y[i];
		Sxy += x[i]*y[i];
		Sxx += x[i]*x[i];
		Syy += y[i]*y[i];
	}
	float denom = (n*Sxx - Sx*Sx);

	if( denom ==0 ){
		return 0;
	}

	slope = (n*Sxy - Sx*Sy)/denom;
	intercept = (Sy - slope*Sx)/n;

	correlation = (n*Sxy - Sx*Sy)/(sqrt(n*Sxx-Sx*Sx)*sqrt(n*Syy-Sy*Sy));
	return 1;
}

float calc_t2_from_two_tes(float s1, float s2, float te1, float te2){
	//s = S0*e(-te/t2)
	//t2 = (te2-te1)/(log(s1/s2));
	//S0 = s1/e(-te1/t2);
	float t2=0;
	if(s2==0){
		t2 = 0;
	}else{
		t2 = (te2-te1)/(log(s1/s2));
	}
	return t2;
}
