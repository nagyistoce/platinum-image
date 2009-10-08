#include "additional_data.h"

additional_data::additional_data(){
	show_up = show_down = false;
}
additional_data::~additional_data(){
}

/*void additional_data::draw_all_data(unsigned char* rgb_map, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type){
	
	for(int i = 0; i<data.size(); i++){
		if(data[i]->points_to_draw.empty()){
			data[i]->calc_data(rgb_map, width, height, rg, type);
		}
		data[i]->draw_it(data[i]->points_to_draw,rgb_map, width, height, rg, type);
		//data[i]->draw_data(rgb_map, width, height, rg, type);
	}
}*/
void additional_data::write_all_data_to_file(string file){
	ofstream myfile(file.c_str());
	if(myfile.is_open()){
		myfile << data.size() << "\n";
		for(int i = 0; i < data.size(); i++){
			data.at(i)->write_data(myfile);
		}
		myfile.close();
	}
}
void additional_data::read_all_data_from_file(string file){
	int nr_items;
	ADDITIONAL_TYPE type;
	int t;
	Vector3D vec1, vec2, vec3, vec4;
	float r1, r2, a;
	string s;

	ifstream myfile(file.c_str());
	if(myfile.is_open()){
		myfile >> nr_items;
		for(int i = 0; i < nr_items; i++){
			cout << i <<"/"<<nr_items<<endl;
			myfile >> t;
			type = (ADDITIONAL_TYPE) t;
			switch(type){
				case AT_POINT:
					myfile >> vec1[0] >> vec1[1] >> vec1[2];
					add_point(vec1);
					break;
				case AT_LINE:
					myfile >> vec1[0] >> vec1[1] >> vec1[2];
					myfile >> vec2[0] >> vec2[1] >> vec2[2];
					add_line(vec1, vec2);
					break;
				case AT_RECT:
					myfile >> vec1[0] >> vec1[1] >> vec1[2];
					myfile >> vec2[0] >> vec2[1] >> vec2[2];
					myfile >> vec3[0] >> vec3[1] >> vec3[2];
					myfile >> vec4[0] >> vec4[1] >> vec4[2];
					add_rect(vec1, vec2, vec3, vec4);
					break;
				case AT_CIRCLE:
					myfile >> vec1[0] >> vec1[1] >> vec1[2];
					myfile >> vec2[0] >> vec2[1] >> vec2[2];
					myfile >> r1 >> r2;
					add_circle(vec1, vec2, r1, r2);
					break;
				case AT_STRING:
					myfile >> vec1[0] >> vec1[1] >> vec1[2];
					myfile >> s;
					add_text(vec1,s);
					break;
				case AT_GAUSS:
					myfile >> r1 >> r2 >> a;
					add_gauss(r1, r2, a);
				default:
					break;
			}
		}
		myfile.close();
	}
}

void additional_data::add_point(Vector3D p){
	data.push_back(new point_data(p));
}
void additional_data::add_circle(Vector3D p, Vector3D n, float r, float r1){
	data.push_back(new circle_data(p, n, r, r1));
}
void additional_data::add_rect(Vector3D c1, Vector3D c2, Vector3D c3, Vector3D c4){
	data.push_back(new rect_data(c1,c2, c3, c4));
}
void additional_data::add_line(Vector3D x1, Vector3D x2){
	data.push_back(new line_data(x1, x2));
}
void additional_data::add_text(Vector3D p, string s){
	data.push_back(new text_data(p, s));
}
void additional_data::add_gauss(float mean, float std, float amp){
	data.push_back(new gauss_data(mean, std, amp));
}
void additional_data::add_freehand(vector<Vector3D> p){
	data.push_back(new freehand_data(p));
}

/* -------------------------------------------- */
/* -------------------------------------------- */

additional_data_base::additional_data_base(){
	
}
/*void additional_data_base::draw_it(vector<Vector3D> points_to_draw, unsigned char* pixels, int width, int height, rendergeometry_base* rg, RENDERER_TYPE type){
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
			//Fixa oversattning har
			point[0] = 0;
			point[1] = 0;
			point[2] = 0;
		}
		point[0] = round(point[0]);
		point[1] = round(point[1]);
		//lagg till sarfall har. Tror det bara ar point som inte vill ritas med linjer
		if(point[0] < width && point[0] > 0 && point[1] < height && point[1] > 0 ){
			
			pix_addr = (point[1]*width + point[0])*RGB_pixmap_bpp;
			pixels[pix_addr] = 255-pixels[pix_addr];
			pixels[pix_addr+1] = 255-pixels[pix_addr+1];
			pixels[pix_addr+2] = 255-pixels[pix_addr+2];
		}
	}

}*/
/* -------------------------------------------- */
/* -------------------------------------------- */

point_data::point_data(Vector3D c) : additional_data_base(){
		p = c;
		type = AT_POINT;
}

void point_data::write_data(ofstream &myfile){
	myfile << type << "\n";
	myfile << p[0] << " " << p[1] << " " << p[2] <<"\n";
}

void point_data::calc_data(){
	
	//vector<Vector3D> points_to_draw;
	points_to_draw.clear();
//	int pix_addr;
	

	points_to_draw.push_back(p);
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

void circle_data::write_data(ofstream &myfile){
	myfile << type << "\n";
	myfile << c[0] << " " << c[1] << " " << c[2] <<"\n";
	myfile << n[0] << " " << n[1] << " " << n[2] <<"\n";
	myfile << radius << "\n" << radius1 << "\n";
}

void circle_data::calc_data(){
	
	//vector<Vector3D> points_to_draw;
	points_to_draw.clear();
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

void line_data::write_data(ofstream &myfile){
	myfile << type << "\n";
	myfile << start[0] << " " << start[1] << " " << start[2] <<"\n";
	myfile << stop[0] << " " << stop[1] << " " << stop[2] <<"\n";
}
void line_data::calc_data(){

	points_to_draw.push_back(start);
	points_to_draw.push_back(stop);


	//this->draw_it(points_to_draw, pixels, width, height, rg, type);
}
/* -------------------------------------------- */
/* -------------------------------------------- */

rect_data::rect_data(Vector3D p1, Vector3D  p2, Vector3D  p3, Vector3D  p4) : additional_data_base(){
		c1 = p1;
		c2 = p2;
		c3 = p3;
		c4 = p4;
		type = AT_RECT;
}

void rect_data::write_data(ofstream &myfile){
	myfile << type << "\n";
	myfile << c1[0] << " " << c1[1] << " " << c1[2] <<"\n";
	myfile << c2[0] << " " << c2[1] << " " << c2[2] <<"\n";
	myfile << c3[0] << " " << c3[1] << " " << c3[2] <<"\n";
	myfile << c4[0] << " " << c4[1] << " " << c4[2] <<"\n";
}
void rect_data::calc_data(){

	points_to_draw.push_back(c1);
	points_to_draw.push_back(c2);
	points_to_draw.push_back(c3);
	points_to_draw.push_back(c4);
	points_to_draw.push_back(c1); //have to close the rectangle
	//this->draw_it(points_to_draw, pixels, width, height, rg, type);
}
/* -------------------------------------------- */
/* -------------------------------------------- */

text_data::text_data(Vector3D c, string text) : additional_data_base(){
		p = c;
		s = text;
		type = AT_STRING;
}

void text_data::write_data(ofstream &myfile){
	myfile << type << "\n";
	myfile << p[0] << " " << p[1] << " " << p[2] << "\n";
	myfile << s << "\n";
}
void text_data::calc_data(){
	points_to_draw.clear();
	points_to_draw.push_back(p);
}


gauss_data::gauss_data(float mean, float std, float amplitude) : additional_data_base(){
		omega = std;
		my = mean;
		amp = amplitude;
		type = AT_GAUSS;
}
void gauss_data::write_data(ofstream &myfile){
	myfile << type << "\n";
	myfile << my<< " " << omega << " " << amp <<"\n";
}
void gauss_data::calc_data(){
	points_to_draw.clear();
	float ans;
	double multi = 1.0;
	//ans = amp*(1/(omega*sqrt(2*pt_PI)))*exp(-(pow(my-my,2)/(2*pow(omega,2))));
	ans = amp* exp( -0.5 * pow((my-my),2)/pow(omega,2) );
	
	Vector3D point;
	point[0] = my;
	//ans = amp*(1/(omega*sqrt(2*pt_PI)))*exp(-(pow(my-my,2)/(2*pow(omega,2))));
	ans = amp* exp( -0.5 * pow((my-my),2)/pow(omega,2) );
	point[1] = ans*multi;
	point[2] = 1;
	points_to_draw.push_back(point);
	
	for(int i = my+1; ans > 0.0002 ; i++){
		point[0] = i;
		//ans = amp*(1/(omega*sqrt(2*pt_PI)))*exp(-(pow(i-my,2)/(2*pow(omega,2))));
		ans = amp* exp( -0.5 * pow((i-my),2)/pow(omega,2) );
		point[1] = ans*multi;
		point[2] = 1;
		points_to_draw.push_back(point);
		point[0] = my + (my-i);
		points_to_draw.insert(points_to_draw.begin(),point);
	}
	//this->draw_it(points_to_draw, pixels, width, height, rg, type);
}


/* -------------------------------------------- */
/* -------------------------------------------- */

freehand_data::freehand_data(vector<Vector3D> c) : additional_data_base(){
		p = c;
		type = AT_FREEHAND;
}

void freehand_data::write_data(ofstream &myfile){
	myfile << type << "\n";
	myfile << p.size() << "\n";
	for(int i = 0; i < p.size(); i++)
		myfile << p.at(i)[0] << " " << p.at(i)[1] << " " << p.at(i)[2] <<"\n";
}

void freehand_data::calc_data(){
	
	//vector<Vector3D> points_to_draw;
	points_to_draw.clear();
//	int pix_addr;
	
	for(int i = 0; i < p.size(); i++)
		points_to_draw.push_back(p.at(i));
	//this->draw_it(points_to_draw, pixels, width, height, rg, type);
	
}
