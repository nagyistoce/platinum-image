//////////////////////////////////////////////////////////////////////////
//
//  header collecting mathematical functions used
//  throughout the platform
//
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

#ifndef __ptmath__
#define __ptmath__

#define pt_PI 3.1415926536
#define pt_golden_section 0.6180339887

#include <limits>	//for example...  numeric_limits<float>
#include <set>

//tensor algebra type defs, dependencies...
#include "itkVector.h"
#include "itkPoint.h"
#include "itkMatrix.h"

// used by fit_points
#include "itkLandmarkBasedTransformInitializer.h"
#include "itkImage.h"

#include "fileutils.h"

class v3i{
	public:
	int data[3];
};


///...and types
typedef itk::Vector<float,3> Vector3D;
typedef itk::Vector<int,3> Vector3Dint;
typedef v3i Vector3Di;
typedef itk::Point<float,3> Point3D;
typedef std::pair<long, long> HistoPair;
typedef itk::Matrix<float> Matrix3D;

typedef itk::Vector<float,2> Vector2D;
typedef itk::Vector<int,2> Vector2Dint;
//typedef itk::Matrix<float,2,2> Matrix2D;


class gaussian{
public:
	gaussian(float amp=0, float cent=0, float sig=0);
	~gaussian(void);
	float amplitude;	
	float center;
	float sigma;		//standard deviation
	float evaluate_at(float x);
	double integrate_total_area(int from_x, int to_x);
	int get_x_that_includes_area_fraction(double fraction, int from_x, int to_x);
	float get_value_at_intersection_between_centers(gaussian g2, float num_calc_steps=200);
};


class gaussian_2d{
public:
	gaussian_2d(float amp=0, float cent_x=0, float cent_y=0, float sig_u=0, float sig_v=0, float angle=0);
	~gaussian_2d(void);
	float amplitude;	
	float center_x;
	float center_y;
	float sigma_u;		//standard deviation
	float sigma_v;
	float phi;			//angle of u and v relative to x and y (radians)
	float evaluate_at(float x, float y);
};

class rayleighian{
private:
	float amp;	
	float sig;
	float sig2;

public:
	rayleighian(float amp_val=0, float sigma_val=0);
	~rayleighian(void);
	float evaluate_at(float x);
	float get_variance();
	float get_std();
};

double gamma(double x);

class line2D{

public:
	line2D();
	line2D(Vector2D pnt1, Vector2D pnt2);
	line2D(float x1, float y1, float x2, float y2);

	Vector2D get_point();
	Vector2D get_direction();
	void set_point(Vector2D pnt);
	void set_point(float point_x, float point_y);
	void set_direction(Vector2D dir);
	void set_direction(float dir_x, float dir_y);

	bool is_point_left_of_line(Vector2D pnt);
	bool is_point_right_of_line(Vector2D pnt);
	bool is_point_left_of_line(float x, float y);
	bool is_point_right_of_line(float x, float y);

protected:
	Vector2D point;
	Vector2D direction;
};

class line3D{ //TODO: A future plan in to put this under data_base and connect useful rendering to it...

public:
	line3D();
	line3D(Vector3D pnt, Vector3D dir);
	line3D(float point_x, float point_y, float point_z, float dir_x, float dir_y, float dir_z);

	Vector3D get_point();
	Vector3D get_direction();
	void set_point(Vector3D pnt);
	void set_point(float point_x, float point_y, float point_z);
	void set_direction(Vector3D dir);
	void set_direction(float dir_x, float dir_y, float dir_z);

	void least_square_fit_line_to_points_in_3D(vector<Vector3D> points, int dir);

	friend std::ostream &operator<<(std::ostream &ut, const line3D &l);


protected:
	Vector3D point;
	Vector3D direction;
	void set_point_to_center_of_gravity_from_points_in_3D(vector<Vector3D> points);
	void set_direction_to_point_cloud_variations_given_one_dir(int dir, vector<Vector3D> points); //note that point needs to be set to center of gravity...
};

class plane3D{
public:
	plane3D();
	plane3D(Vector3D pnt, Vector3D norm);
	plane3D(float point_x, float point_y, float point_z, float normal_x, float normal_y, float normal_z);
	plane3D(Vector3D point1, Vector3D point2, Vector3D point3);
	
	Vector3D get_point();
	Vector3D get_normal();
	bool is_defined();
	void set_point(Vector3D pnt);
	void set_point(float point_x, float point_y, float point_z);
	void set_normal(Vector3D norm);
	void set_normal(float normal_x, float normal_y, float normal_z);

	void invert();
	Vector3D get_point_of_intersection(line3D line);
	line3D get_line_of_intersection(plane3D plane);
	line2D get_projected_line(line3D line);

	friend std::ostream &operator<<(std::ostream &ut, const plane3D &p);

protected:
	Vector3D point;
	Vector3D normal;
	bool defined; //Undefined planes can be used as default values for functions taking a varying number of planes as arguemt
};

class ellipsoid3D{
public:
	ellipsoid3D();
	ellipsoid3D(Vector3D centrum, Vector3D radii, float azimuth=0, float elevation=0);
	ellipsoid3D(Vector3D centrum, float radius_u, float radius_v, float radius_w, float azimuth=0, float elevation=0);
	ellipsoid3D(float centrum_x, float centrum_y, float centrum_z, float radius_u, float radius_v, float radius_w, float azimuth=0, float elevation=0);

	Vector3D get_centrum();
	Vector3D get_radii();
	float get_azimuth();
	float get_elevation();
	void set_centrum(Vector3D centrum);
	void set_centrum(float centrum_x, float centrum_y, float centrum_z);
	void set_radii(Vector3D scale);
	void set_radii(float radius_u, float radius_v, float radius_w);
	void set_azimuth(float azimuth);
	void set_elevation(float elevation);

	bool is_point_inside_ellipsoid(float x, float y, float z);
	bool is_point_inside_ellipsoid(Vector3D pnt);

protected:
	Vector3D _centrum;
	Vector3D _radii;
	float _azimuth;
	float _elevation;
};

bool convex_last_three_points(vector<Vector2D>::iterator end, bool lower); // used by get_convex_hull_2D
vector<Vector2D> get_convex_hull_2D(vector<Vector2D> points); // points should be sorted, first by x, then by y (first and second values in Vector2D)

//not typical tensor functions, just for lack of a better place

template<class T>                   
void t_swap(T & t1, T & t2){        //templated swap functions that's less
                                    //(not at all) picky on input
	T tmp(t1); 
	t1 = t2; 
	t2  = tmp; 
}

template <class T>
T round (T f)   //round to nearest integer
{return floor (f + 0.5); }

template <class T>
T abs_ceil (T f)   //round to nearest higher, ignoring sign
{
	T sign = (f < 0 ? -1 : 1);
	return sign*ceil (f*sign);
}

template <class sType, unsigned int vDim>
float min_norm(itk::Vector<sType, vDim> &V)     //normalize vector using minimum norm
{
	sType m = V[0];
	for (unsigned int d=0;d < vDim;d++){
		m = min(m,V[d]);
	}
    return m;
}

template <class sType, unsigned int vDim>
void min_normalize(itk::Vector<sType, vDim> &V)     //normalize vector using minimum norm
{
	V /= min_norm<sType,vDim>(V);
}

template <class sType, unsigned int vDim>
float max_norm (itk::Vector<sType, vDim> &V)
{
	sType m = V[0];
	for (unsigned int d=0;d < vDim;d++){
		m=max(m,V[d]);
    }
    return m;
}


template <class C, class D> void adjust_endian (D* data, C size,bool DataIsBigEndian)
//alter endian if supplied value
//and system value don't match
{
	bool RunsOnBigEndian;
    
#ifdef _MSC_VER
	short word = 0x4321;
	RunsOnBigEndian = (*(char *)& word) != 0x21 ;
#else
	RunsOnBigEndian = (htonl(1)==1 );
#endif
    
	unsigned int swapSize = sizeof (D);
    
	if (RunsOnBigEndian ^ DataIsBigEndian)
        {
		for (C p =0;p < size;p++)
            {
			unsigned char * b = (unsigned char *) &(data[p]);
			register int i = 0;
			register int j = swapSize-1;
            
			while (i<j)
                {
				t_swap(b[i], b[j]);
				i++, j--;
                }
            }
        }
}


//templated string to num conversion
template <class T>
bool dec_from_string(T& outType, const std::string& inString)
    {
    std::istringstream convstream(inString);
    return !(convstream >> std::dec >> outType).fail();
    }


/*
 bool has_only_positive_or_zero_components(Vector3D v)
 {
     for(int i=0;i<=2;i++)
         {
         if(v[i]<0)
             return false;
         }
     return true;
 }
 
 bool has_only_negative_or_zero_components(Vector3D v)
 {
     for(int i=0;i<=2;i++)
         {
         if(v[i]>0)
             return false;
         }
     return true;
 }
 */



/*
class matrix_generator{
public:
	Matrix3D get_rot_x_matrix_3D(float fi);			//fi in radians
	Matrix3D get_rot_y_matrix_3D(float fi);			//fi in radians
	Matrix3D get_rot_z_matrix_3D(float fi);			//fi in radians
	
	//rotation examples based on the "basic" image processing coordinate system
	//(x-->right, y-->down and z--> the view direction of the screen)
	//+fi_z rotates the image volume: "Counterclockwise" of the z-direction
	//+fi_y rotates the image volume: "Counterclockwise" of the y-direction
	//+fi_x rotates the image volume: "Counterclockwise" of the x-direction
	Matrix3D get_rot_matrix_3D(float fi_z, float fi_y, float fi_x);	//fi_z/y/x in radians
	Matrix3D get_rot_matrix_3D(int fi_z, int fi_y, int fi_x);	//fi_z/y/x in degrees
};
*/

//Following rules can be noted about the rotation matrices below:
// R^(-1)(fi) = R(-fi)
// R^(-1)(fi) = R^T(fi)
// R-total = RzRyRx --> R^(-1) = R^(T)

//rotation examples based on the "basic" image processing coordinate system
//(x-->right, y-->down and z--> the view direction of the screen)
//+fi_z rotates the image volume: "Counterclockwise" of the z-direction
//+fi_y rotates the image volume: "Counterclockwise" of the y-direction
//+fi_x rotates the image volume: "Counterclockwise" of the x-direction

Matrix3D create_rot_x_matrix_3D(float fi_rad);									//fi in radians
Matrix3D create_rot_y_matrix_3D(float fi_rad);									//fi in radians
Matrix3D create_rot_z_matrix_3D(float fi_rad);									//fi in radians
Matrix3D create_rot_matrix_3D(float fi_x_rad, float fi_y_rad, float fi_z_rad);	//fi_x/y/z in radians




///////////////////////////////////////////////////////
// Given x and y vectors of length nSteps, returns 
// a vector of second derivatives, y2 (natural cubic spline).  Based 
// on the routine "spline" from Numerical Recipes section 3.3.
// First, call pt_spline once to generate y2.
// Then call pt_splint (using y2) to  interpolate values...

void pt_spline1D(float x[],float y[],int n,float yp1,float ypn,float y2[]);
float pt_splint1D(float xa[],float ya[],float y2a[],int n,float x);




// Tricubic interpolation using method described in:
// F. Lekien, J.E. Marsden
// Tricubic Interpolation in Three Dimensions
// International Journal for Numerical Methods in Engineering, 63 (3), 455-471, 2005
// ...used in the function: image_scalar.interpolate_tricubic_3D...

extern const int A_tricubic[64][64]; //defined in ptmath.cc

template <class T>
void print_type_limits()
{
	std::cout<<std::numeric_limits<T>::min()<<"\t";
	std::cout<<std::numeric_limits<T>::max()<<"\t";
	std::cout<<std::numeric_limits<T>::digits<<std::endl;
}

template <class ELEMTYPE>
float pt_log(float x, ZERO_HANDLING_TYPE zht)
{
	if(x>0){
		return x*log(x);
	}

	if(zht==ZERO_HANDLING_LEAVE){
		return x*log(x);
	}
	else if (zht==ZERO_HANDLING_SET_ZERO) {
		return 0;
	}
	else if (zht==ZERO_HANDLING_SET_MIN) {
		return std::numeric_limits<ELEMTYPE>::min();
	}

	return 0;
}

float pt_LoG(float x, float sigma);


void print_datatype_numerical_limits();
double get_random_number_in_span(double min, double max);
Vector3D create_Vector3D(float x, float y, float z);
Vector3D create_Vector3D(vnl_vector<float> &v);
Vector3Dint create_Vector3Dint(int x, int y, int z);
Vector3Di create_Vector3Di(int x, int y, int z);
Vector3Dint create_Vector3Dint(Vector3D v);
Vector2D create_Vector2D(float x, float y);
Vector2Dint create_Vector2Dint(int x, int y);
void save_Vector3D_to_file(Vector3D v,string filepath);
void save_vector_of_Vector3D_to_file(vector<Vector3D> v,string filepath);
Vector3D load_Vector3D_from_file(string filepath);
Vector3D string2Vector3D(string s);
vector<Vector3D> load_vector_of_Vector3D_from_file(string filepath);
bool is_defined(float f);
bool is_defined(Vector3D v);

int get_component_from_dir(int x, int y, int z, int dir);
float get_component_from_dir(float x, float y, float z, int dir);
float get_component_from_dir(Vector3D &v, int dir);

int get_component_from_dim_and_dir(int x, int y, int z, int dim, int dir);
float get_component_from_dim_and_dir(float x, float y, float z, int dim, int dir);
float get_component_from_dim_and_dir(Vector3D &v, int dim, int dir);

Vector3D get_mean_Vector3D(vector<Vector3D> v);
Vector3Dint get_mean_Vector3Dint(vector<Vector3D> v);
string Vector3D2str(Vector3D v);
string bool2str(bool b);

Matrix3D outer_product(const Vector3D a, const Vector3D b);
void copy_columns(Matrix3D &m_to, Matrix3D &m_from, int to_col, int from_col, bool negate=false);
string Matrix3D2str(Matrix3D m);
string Matrix3Drow2str(Matrix3D m, int row);

unsigned int get_factorial(unsigned int i);
unsigned int get_permutations(unsigned int n, unsigned int r); //returns pascals triangle values
unsigned int get_smallest_power_above(unsigned int this_val, unsigned int power_base=2); //e.g. this_val=10 (base=2) --> 16

// mean
template <class T>
double mean(const std::vector<T> & v)
{
	double sum = 0.0;
	for (typename std::vector<T>::const_iterator itr = v.begin(); itr != v.end(); itr++)
		{ sum += *itr; }
	return sum / v.size(); 
}

// variance
template <class T>
double var(const std::vector<T> & v)
{
	double m = mean(v);
	double sum = 0.0;
	for (typename std::vector<T>::const_iterator itr = v.begin(); itr != v.end(); itr++)
		{ sum += (*itr - m) * (*itr - m); }
	return sum / (v.size() - 1);
}

// standard deviation
template <class T>
double sd(const std::vector<T> & v)
{
	return sqrt(var(v)); 
}

float magnitude(const Vector3D v);
float magnitude(const Vector3Dint v);
Vector3D mean(const std::vector<Vector3D> & x);
Matrix3D var(const std::vector<Vector3D> & x);
Matrix3D pooled_cov(const std::vector<Vector3D> & x, const std::vector<Vector3D> & y);
float tsquare(const std::vector<Vector3D> & x, const std::vector<Vector3D> & y);		// Hotelling's two-sample t-square statistic

void subtract_from_all(std::vector<Vector3D> & x, Vector3D value);
Matrix3D cov(std::vector<Vector3D> x, std::vector<Vector3D> y);
Matrix3D cov2(std::vector<Vector3D> x, std::vector<Vector3D> y); //with scaled vectors (where mean is subtracted)

// Some parts of the implementation of some the following algorithms were inspired by
// "Numerical Recipes in C", Second Edition, Press, Teukolsky, Vetterling, Flannery
double invF(const double p, const double a, const double b);				// Inverse cumulative F distribution function
double invIncompleteBeta(const double p, const double a, const double b);	// Inverse incomplete beta function
double logGamma(const double x);											// Logarithm of the Gamma function
double incompleteBeta(const double a, const double b, const double x);		// Incomplete beta function
double incompleteBetaCF(const double a, const double b, const double x);	// Continued fraction for incomplete beta function
double regularizedGamma(const unsigned int N, const double x);				// Regularized Gamma function

// Rigid 3D landmark based alignment
// Three non-collinear landmarks is sufficient to guarantee a solution
// fixedToMoving usage: fixedToMoving[index_of_fixed] = index_of_moving
bool fit_points(const std::vector<Vector3D> & fixed, std::vector<Vector3D> & moving, const std::map<int,int> & fixedToMoving);
float ttest_indep_2sample_equal_variance(vector<float> &group_a, vector<float> &group_b);


int fit_line(vector<float> &x, vector<float> &y, float &slope, float &intercept, float &correlation);		// Least square fit line to 2D data.

#endif	//__ptmath.h__