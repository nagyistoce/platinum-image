#include "additional_data.h"

additional_data::additional_data(){
}
additional_data::~additional_data(){
}

void additional_data::draw_all_data(unsigned char* rgb_map, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type){
	
	for(int i = 0; i<data.size(); i++){
		if(data[i]->points_to_draw.empty()){
			data[i]->calc_data(rgb_map, width, height, rg, type);
		}
		data[i]->draw_it(data[i]->points_to_draw,rgb_map, width, height, rg, type);
		//data[i]->draw_data(rgb_map, width, height, rg, type);
	}
}

void additional_data::add_point(Vector3D p, int size){
	data.push_back(new point_data(p, size));
}
void additional_data::add_circle(Vector3D p, Vector3D n, float r, float r1){
	data.push_back(new circle_data(p, n, r, r1));
}
void additional_data::add_rect(Vector3D c1, Vector3D c2, Vector3D c3, Vector3D c4){
	data.push_back(new line_data(c1,c2));
	data.push_back(new line_data(c2,c3));
	data.push_back(new line_data(c3,c4));
	data.push_back(new line_data(c4,c1));
}
void additional_data::add_line(Vector3D x1, Vector3D x2){
	data.push_back(new line_data(x1, x2));
}

/* -------------------------------------------- */
/* -------------------------------------------- */

additional_data_base::additional_data_base(){
	
}
void additional_data_base::draw_it(vector<Vector3D> points_to_draw, unsigned char* pixels, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type){
	int pix_addr;
	//inside
	
	for(int i = 0; i < points_to_draw.size(); i++){
		Vector3D point;
		if(type == RENDERER_CURVE){
			rendergeom_curve* geom = dynamic_cast<rendergeom_curve*>(rg);
			//Allt kanske skiter sig om man inte har en kurva satt i geometrin...
			point = geom->curve_to_view(points_to_draw.at(i)[0], points_to_draw.at(i)[1], width, height);	
		}else if(type ==RENDERER_MPR){
			rendergeom_image* g = dynamic_cast<rendergeom_image*>(rg);
			//Fixa översättning här
			point[0] = 0;
			point[1] = 0;
			point[2] = 0;
		}
		point[0] = round(point[0]);
		point[1] = round(point[1]);
		//lägg till särfall här. Tror det bara är point som inte vill ritas med linjer
		if(point[0] < width && point[0] > 0 && point[1] < height && point[1] > 0 ){
			
			pix_addr = (point[1]*width + point[0])*RGB_pixmap_bpp;
			pixels[pix_addr] = 255-pixels[pix_addr];
			pixels[pix_addr+1] = 255-pixels[pix_addr+1];
			pixels[pix_addr+2] = 255-pixels[pix_addr+2];
		}
	}

}
/* -------------------------------------------- */
/* -------------------------------------------- */

point_data::point_data(Vector3D c, int size) : additional_data_base(){
		p = c;
		point_size = size;
		type = AT_POINT;
}

void point_data::draw_data(unsigned char* pixels, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type){
	fl_color(FL_BLACK);
	int b = round(point_size/2.0);
	fl_rectf(p[0]-b,p[1]-b,point_size,point_size);
}

void point_data::calc_data(unsigned char* pixels, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type){
	
	//vector<Vector3D> points_to_draw;
	points_to_draw.clear();
//	int pix_addr;
	
	int span = floor(point_size/2.0);
	for(int i = -span; i<=span; i++){
		for(int j = -span; j<=span; j++){
			for(int k = -span; k<=span; k++){
				Vector3D point;
				point[0] = p[0]+i;
				point[1] = p[1]+j;
				point[2] = p[2]+k;
				points_to_draw.push_back(point);
			}
		}
	}
	//this->draw_it(points_to_draw, pixels, width, height, rg, type);
	
}


/* -------------------------------------------- */
/* -------------------------------------------- */

circle_data::circle_data(Vector3D p, Vector3D normal, float r, float r1) : additional_data_base(){
		c= p;
		n = normal;
		radius = r;
		if(r1 == -1)
			radius1 = radius;
		else
			radius1 = r1;
		type = AT_CIRCLE;
}

void circle_data::draw_data(unsigned char* pixels, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type){
	fl_color(FL_BLACK);
	fl_arc(c[0], c[1],radius,0,360);
	fl_arc(10, 10,10,0,360);
	fl_line(10,10,500,500);
	cout << "drawing circle" << endl;
}

void circle_data::calc_data(unsigned char* pixels, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type){
	
	//vector<Vector3D> points_to_draw;
	points_to_draw.clear();
	int pix_addr;
	double factor;
	Vector3D w,u,v;
	double pi = 3.14159265;

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
 
	double step_length = (2*pi)/16;
	for(double i = 0.0; i <= 2*pi+step_length; i+=step_length){
		Vector3D point;
		point[0] = c[0] + (radius*cos(i))*u[0] + (radius1*sin(i))*v[0];
		point[1] = c[1] + (radius*cos(i))*u[1] + (radius1*sin(i))*v[1];
		point[2] = c[2] + (radius*cos(i))*u[2] + (radius1*sin(i))*v[2];
		points_to_draw.push_back(point);
	}

	//X(t) = C + (r*cos(t))*U + (r*sin(t))*V

	//this->draw_it(points_to_draw, pixels, width, height, rg, type);
	
}


/* -------------------------------------------- */
/* -------------------------------------------- */

line_data::line_data(Vector3D p1, Vector3D  p2) : additional_data_base(){
		start = p1;
		stop = p2;
		type = AT_LINE;
}

void line_data::draw_data(unsigned char* pixels, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type){
	fl_color(FL_BLACK);
	fl_line(start[0],start[1], stop[0], stop[1]);
}

void line_data::calc_data(unsigned char* pixels, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type){
	//vector<Vector3D> points_to_draw;
	/*points_to_draw.clear();
	int i, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
    Vector3D pixel;

	pixel = start;
    dx = stop[0] - start[0];
    dy = stop[1] - start[1];
    dz = stop[2] - start[2];

    x_inc = (dx < 0) ? -1 : 1;
    l = abs(dx);
    y_inc = (dy < 0) ? -1 : 1;
    m = abs(dy);
    z_inc = (dz < 0) ? -1 : 1;
    n = abs(dz);
    dx2 = l << 1;
    dy2 = m << 1;
    dz2 = n << 1;

    if ((l >= m) && (l >= n)) {
        err_1 = dy2 - l;
        err_2 = dz2 - l;
        for (i = 0; i < l; i++) {
			points_to_draw.push_back(pixel);
            if (err_1 > 0) {
                pixel[1] += y_inc;
                err_1 -= dx2;
            }
            if (err_2 > 0) {
                pixel[2] += z_inc;
                err_2 -= dx2;
            }
            err_1 += dy2;
            err_2 += dz2;
            pixel[0] += x_inc;
        }
    } else if ((m >= l) && (m >= n)) {
        err_1 = dx2 - m;
        err_2 = dz2 - m;
        for (i = 0; i < m; i++) {
            points_to_draw.push_back(pixel);
            if (err_1 > 0) {
                pixel[0] += x_inc;
                err_1 -= dy2;
            }
            if (err_2 > 0) {
                pixel[2] += z_inc;
                err_2 -= dy2;
            }
            err_1 += dx2;
            err_2 += dz2;
            pixel[1] += y_inc;
        }
    } else {
        err_1 = dy2 - n;
        err_2 = dx2 - n;
        for (i = 0; i < n; i++) {
            points_to_draw.push_back(pixel);
            if (err_1 > 0) {
                pixel[1] += y_inc;
                err_1 -= dz2;
            }
            if (err_2 > 0) {
                pixel[0] += x_inc;
                err_2 -= dz2;
            }
            err_1 += dy2;
            err_2 += dx2;
            pixel[2] += z_inc;
        }
    }
    points_to_draw.push_back(pixel);*/
	points_to_draw.push_back(start);
	points_to_draw.push_back(stop);


	//this->draw_it(points_to_draw, pixels, width, height, rg, type);
}
/* -------------------------------------------- */
/* -------------------------------------------- */

text_data::text_data(Vector3D c, string text) : additional_data_base(){
		p = c;
		s = text;
		type = AT_STRING;
}
void text_data::draw_data(unsigned char* pixels, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type){
	fl_color(FL_BLACK);
	fl_draw(s.c_str(),p[0],p[1]);
}

void text_data::calc_data(unsigned char* pixels, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type){

}