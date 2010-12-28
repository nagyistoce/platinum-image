#include "Mesh.h"

Mesh::Mesh(image_binary<3> *start, int res){
	//start->outline_3D();
	int x1,y1,z1,x2,y2,z2;
	id = 0;
	vector<Vector3D> points;
	start->get_smallest_box_containing_entire_object_3D(x1,y1,z1,x2,y2,z2);
	bool first = true;
	Vector3Dint to, from, last_to = create_Vector3Dint(0,0,0), last_from = create_Vector3Dint(0,0,0);
	cout << "x1: " << x1 << "  x2: " << x2 << endl;
	cout << "y1: " << y1 << "  y2: " << y2 << endl;
	cout << "z1: " << z1 << "  z2: " << z2 << endl;
	for(int z = z2+1; z < z1-1; z+=res){
		cout << "z: " << z << endl;
		start->get_span_of_value_in_subregion_3D(1,create_Vector3Dint(x2-1,y2-1,z),create_Vector3Dint(x1+1,y1+1,z),from,to);
		if(first){
			points.push_back(from);
			points.push_back(create_Vector3Dint(to[0],from[1],from[2]));
			points.push_back(to);
			points.push_back(create_Vector3Dint(from[0],to[1],to[2]));
			add_polygon(points);
			first = false;
			points.clear();
		}else{
			points.push_back(last_from);//1
			points.push_back(from); //2
			points.push_back(create_Vector3Dint(to[0],from[1],from[2])); //3
			points.push_back(create_Vector3Dint(last_to[0],last_from[1],last_from[2])); //4
			add_polygon(points);
			points.clear();

			points.push_back(last_from);//1
			points.push_back(create_Vector3Dint(last_from[0],last_to[1],last_to[2]));//8
			points.push_back(create_Vector3Dint(from[0],to[1],to[2])); //7
			points.push_back(from); //2
			add_polygon(points);
			points.clear();

			points.push_back(create_Vector3Dint(last_to[0],last_from[1],last_from[2])); //4
			points.push_back(create_Vector3Dint(to[0],from[1],from[2])); //3
			points.push_back(create_Vector3Dint(to[0],to[1],from[2])); //5
			points.push_back(create_Vector3Dint(last_to[0],last_to[1],last_from[2])); //6
			add_polygon(points);
			points.clear();

			points.push_back(create_Vector3Dint(last_to[0],last_to[1],last_from[2])); //6
			points.push_back(create_Vector3Dint(last_from[0],last_to[1],last_to[2]));//8
			points.push_back(create_Vector3Dint(from[0],to[1],to[2])); //7
			points.push_back(create_Vector3Dint(to[0],to[1],from[2])); //5
			add_polygon(points);
			points.clear();
		}
		last_to = to;
		last_from = from;
	}
	//Final stopping;
	points.push_back(from);
	points.push_back(create_Vector3Dint(to[0],from[1],from[2]));
	points.push_back(to);
	points.push_back(create_Vector3Dint(from[0],to[1],to[2]));
	add_polygon(points);
	//init_cube(x1,x2,y1,y2,z1,z2);
}
Mesh::Mesh(MESH_SHAPE, int radius, int center){
	id = 0;
	int min = center-radius;
	int max = center+ radius;
	init_cube(min,max,min,max,min,max);
}

Mesh::Mesh(string f, int radius, Vector3D center){
	id = 0;
	parse_file(f);
	for(int i = 0; i < vertices.size(); i++){
		vertices.at(i)->pos[0] = vertices.at(i)->pos[0]*radius + center[0];
		vertices.at(i)->pos[1] = vertices.at(i)->pos[1]*radius + center[1];
		vertices.at(i)->pos[2] = vertices.at(i)->pos[2]*radius + center[2];
	}
}

void Mesh::init_cube(int x1, int x2, int y1, int y2, int z1, int z2){
	Vector3D p1 = create_Vector3D(x1,y1,z1);
	Vector3D p2 = create_Vector3D(x2,y1,z1);
	Vector3D p3 = create_Vector3D(x2,y2,z1);
	Vector3D p4 = create_Vector3D(x1,y2,z1);
	Vector3D p5 = create_Vector3D(x1,y1,z2);
	Vector3D p6 = create_Vector3D(x2,y1,z2);
	Vector3D p7 = create_Vector3D(x2,y2,z2);
	Vector3D p8 = create_Vector3D(x1,y2,z2);

	vector<Vector3D> points;
	points.push_back(p1); points.push_back(p2); points.push_back(p3); points.push_back(p4);
	add_polygon(points);
	points.clear();
	points.push_back(p5); points.push_back(p6); points.push_back(p7); points.push_back(p8);
	add_polygon(points);
	points.clear();
	points.push_back(p1); points.push_back(p2); points.push_back(p6); points.push_back(p5);
	add_polygon(points);
	points.clear();
	points.push_back(p3); points.push_back(p4); points.push_back(p8); points.push_back(p7);
	add_polygon(points);
	points.clear();
	points.push_back(p1); points.push_back(p4); points.push_back(p8); points.push_back(p5);
	add_polygon(points);
	points.clear();
	points.push_back(p2); points.push_back(p3); points.push_back(p7); points.push_back(p6);
	add_polygon(points);
	points.clear();
}

edge* Mesh::does_edge_exist(Vector3D a, Vector3D b, vertex *a_ex, vertex *b_ex){
	int point;
	for(int i = 0; i < edges.size(); i++){
		point = 0;
		if(a == edges.at(i)->v1->pos){
			point++;
			a_ex = edges.at(i)->v1;
		}
		else if(a == edges.at(i)->v2->pos){
			point++;
			a_ex = edges.at(i)->v2;
		}
		if(b == edges.at(i)->v1->pos){
			point++;
			b_ex = edges.at(i)->v1;
		}else if(b == edges.at(i)->v2->pos){
			point++;
			b_ex = edges.at(i)->v2;
		}
		if(point > 1)
			return edges.at(i);
	}
	return NULL;
}

edge* Mesh::add_edge(Vector3D a, Vector3D b){
	edge* e = NULL;
	vertex* v1_ex=NULL;
	vertex* v2_ex=NULL;
	if((e = does_edge_exist(a,b, v1_ex, v2_ex)) != NULL){
		//cout << "edge " << a << " " << b << " exists" << endl;
		return e;
	}
	//cout << "edge " << a << " " << b << " created" << endl;
	/*create edge*/
	e = new edge;

	/*Create vertices*/
	if(v1_ex == NULL){
		v1_ex = new vertex;
		v1_ex->pos = a;
		v1_ex->id = vertices.size();
		id++;
		vertices.push_back(v1_ex);
	}
	if(v2_ex == NULL){
		v2_ex = new vertex;
		v2_ex->pos = b;
		v2_ex->id=vertices.size();
		id++;
		vertices.push_back(v2_ex);
	}

	e->v1 = v1_ex;
	e->v2 = v2_ex;
	//Vertices added in vector
	e->v1->neighbors.push_back(v2_ex);
	e->v2->neighbors.push_back(v1_ex);
	e->f1 = NULL;
	e->f2 = NULL;
	edges.push_back(e);
	//new edge added in vector
	return e;
}

void Mesh::add_polygon(vector<Vector3D> v){
	face* f = new face;
	int max = v.size();
	for(int i = 0; i < max; i++){
		//cout << "adding" << v.at(i)<< " " << v.at((i+1)%max) << endl;
		edge *e = add_edge(v.at(i),v.at((i+1)%max));
		//cout << "Got " << e->v1->pos << " " << e->v2->pos << endl;
		//cout << "------------------------------" << endl;
		f->edges.push_back(e);
		if(e->f1 == NULL)
			e->f1 = f;
		else
			e->f2 = f;
	}
	faces.push_back(f);
	//new polygon added in vector
}

void Mesh::replace_polygon(face *f, vector<Vector3D> v){
	f->edges.clear();
	int max = v.size();
	for(int i = 0; i < max; i++){ 
		edge *e = add_edge(v.at(i),v.at((i+1)%max));
		f->edges.push_back(e);
		if(e->f1 == NULL)
			e->f1 = f;
		else
			e->f2 = f;
	}
}

void Mesh::split_edges(){
	
	for(int i = 0; i < edges.size(); i++){
		vertex *v = new vertex;		
		v->pos = (edges.at(i)->v2->pos + edges.at(i)->v1->pos)/2;
		edges.at(i)->v1->neighbors.clear();
		edges.at(i)->v2->neighbors.clear();
		v->id = vertices.size();
		edges.at(i)->middle = v->id;
		vertices.push_back(v);
	}
}

vector<int> Mesh::divide_polygon(face *f){
	Vector3D cog = create_Vector3D(0,0,0);
	for(int i = 0; i < f->edges.size(); i++){
		cog += f->edges.at(i)->v1->pos;
		cog += f->edges.at(i)->v2->pos;
	}
	cog /= f->edges.size()*2;

	vertex* COG = new vertex;
	COG->pos = cog;
	COG->id = vertices.size();
	vertices.push_back(COG);

	vector<int> points;
	Vector3D t_p, d_p;
	vector<int> all_p;
	bool run;

	all_p.push_back(f->edges.at(0)->v1->id);
	all_p.push_back(f->edges.at(0)->middle);
	all_p.push_back(f->edges.at(0)->v2->id);

	int orig_id;
	orig_id = f->edges.at(0)->v1->id;

	do{
		vertex *x = find_next_vertex(f,all_p.at(all_p.size()-1),all_p.at(all_p.size()-3));
		if(x == NULL)
			cout << "vertex looked for: " << all_p.at(all_p.size()-1)<< "  and " << all_p.at(all_p.size()-3) << " not looked for" << endl;
		run = x->id != orig_id;

		//t_p = (all_p.at(all_p.size()-1) + x->pos)/2;
		all_p.push_back(edge_exists(all_p.at(all_p.size()-1),x->id)->middle);
		all_p.push_back(x->id);
	}while(run);

	all_p.pop_back();
	all_p.insert(all_p.begin(),all_p.back());

	all_p.push_back(COG->id);
	return all_p;
	/*
	*/
}

void Mesh::divide_all_polygons(){
	int s = faces.size();
	split_edges();
	vector<vector<int> > polygons;
	for(int f = 0; f < s; f++){
		polygons.push_back(divide_polygon(faces.at(f)));
	}
	for(int g = 0; g < edges.size(); g++)
		delete edges.at(g);
	for(int u = 0; u < faces.size(); u++)
		delete faces.at(u);
	edges.clear();
	faces.clear();
	
	for(int q = 0; q < polygons.size(); q++){
		vector<int> all_p = polygons.at(q);
		int COG_id = all_p.back();
		all_p.pop_back();
		for(int i = 0; i < all_p.size()-1; i+=2){
			vector<int> points;
			points.push_back(all_p[i]);
			points.push_back(all_p[i+1]);
			points.push_back(all_p[i+2]);
			points.push_back(COG_id);

			
			face *f = new face;
			for(int a = 0; a < points.size(); a++){
				
				edge *e;
				if((e =edge_exists(points.at(a),points.at((a+1)%points.size()))) == NULL){
					e = new edge;
					e->middle = -1;
					e->f1 = f;
					e->f2 = NULL;
					e->v1 = vertices.at(points.at(a));
					e->v2 = vertices.at(points.at((a+1)%points.size()));

					e->v1->neighbors.push_back(e->v2);
					e->v2->neighbors.push_back(e->v1);

					edges.push_back(e);
				}else{
					e->f2 = f;
				}
				f->edges.push_back(e);
			}
			faces.push_back(f);
			points.clear();
		}
	}

}

vertex* Mesh::find_next_vertex(face *f, int find, int no_find){
	for(int i = 0; i < f->edges.size(); i++){
		if(f->edges.at(i)->v1->id == find && f->edges.at(i)->v2->id != no_find)
			return f->edges.at(i)->v2;
		else if(f->edges.at(i)->v2->id == find && f->edges.at(i)->v1->id != no_find)
			return f->edges.at(i)->v1;
		//cout << "id: " <<  f->edges.at(i)->v1->id << " " << f->edges.at(i)->v2->id << endl;
	}
	return NULL;
}
void Mesh::square_divide_polygon(face *f){
	Vector3D cog = create_Vector3D(0,0,0);
	for(int i = 0; i < f->edges.size(); i++){
		cog += f->edges.at(i)->v1->pos;
		cog += f->edges.at(i)->v2->pos;
	}
	cog[0] /= f->edges.size()*2;
	cog[1] /= f->edges.size()*2;
	cog[2] /= f->edges.size()*2;

	vector<Vector3D> points;
	Vector3D t_p, d_p;
	vector<Vector3D> all_p;
	bool run;

	all_p.push_back(f->edges.at(0)->v1->pos);

	t_p[0] = (f->edges.at(0)->v2->pos[0] + f->edges.at(0)->v1->pos[0])/2;
	t_p[1] = (f->edges.at(0)->v2->pos[1] + f->edges.at(0)->v1->pos[1])/2;
	t_p[2] = (f->edges.at(0)->v2->pos[2] + f->edges.at(0)->v1->pos[2])/2;
	all_p.push_back(t_p);
	all_p.push_back(f->edges.at(0)->v2->pos);

	int index_a, index_b, orig_id;
	orig_id = f->edges.at(0)->v1->id;

	index_a = f->edges.at(0)->v1->id;
	index_b = f->edges.at(0)->v2->id;

	do{
		vertex *x = find_next_vertex(f,index_b,index_a);
		if(x == NULL){
			cout << "CRAP..." << endl;
			cout << "vertex looked for: " << index_b << "  and " << index_a << " not looked for" << endl;
		}else{
			cout << "ok" << endl;
		}
		run = x->id != orig_id;
		t_p[0] = (all_p.at(all_p.size()-1)[0] + x->pos[0])/2;
		t_p[1] = (all_p.at(all_p.size()-1)[1] + x->pos[1])/2;
		t_p[2] = (all_p.at(all_p.size()-1)[2] + x->pos[2])/2;
		all_p.push_back(t_p);
		all_p.push_back(x->pos);
		index_a = index_b;
		index_b = x->id;
	}while(run);
	all_p.pop_back();
	all_p.insert(all_p.begin(),all_p.back());
	for(int i = 0; i < all_p.size()-1; i+=2){
		points.push_back(all_p[i]);
		points.push_back(all_p[i+1]);
		points.push_back(all_p[i+2]);
		points.push_back(cog);

		add_polygon(points);
		points.clear();
	}
}
void Mesh::square_divide_all_polygons(){
	vector<face*> new_faces(faces);

	faces.clear();
	edges.clear();
	vertices.clear();

	int s = new_faces.size();
	for(int f = 0; f < s; f++){
		square_divide_polygon(new_faces.at(f));
	}
}

Vector3D Mesh::calc_mean_of_neighbors(vertex *v){
	Vector3D p = create_Vector3D(0,0,0);
	for(int i = 0;  i< v->neighbors.size(); i++){
		p+=v->neighbors.at(i)->pos;
	}
	p[0]/= v->neighbors.size();
	p[1]/= v->neighbors.size();
	p[2]/= v->neighbors.size();
	return p;
}


vector<face*> Mesh::get_all_faces_with_edges(vector<edge*> e){
	vector<face*> fs;
	bool add_f1, add_f2;
	for(int i = 0; i < e.size(); i++){
		add_f1 = add_f2 = true;
		for(int a = 0; a < fs.size(); a++){
			if(fs.at(a) == e.at(i)->f1)
				add_f1 = false;
			if(fs.at(a) == e.at(i)->f2)
				add_f2 = false;
		}
		if(add_f1)
			fs.push_back(e.at(i)->f1);
		if(add_f2)
			fs.push_back(e.at(i)->f2);
	}
	return fs;
}
vector<edge*> Mesh::get_all_edges_with_vertex_id(int id){
	vector<edge*> es;
	for(int i = 0; i <edges.size(); i++){
		if(edges.at(i)->v1->id == id || edges.at(i)->v1->id == id)
			es.push_back(edges.at(i));
	}
	return es;
}

Vector3D Mesh::get_normal_to_vertex(vertex *v){
	Vector3D n = create_Vector3D(0,0,0);
	//edge *e;
	vector<face*> fs;
	vector<edge*> es = get_all_edges_with_vertex_id(v->id);
	fs = get_all_faces_with_edges(es);
	for(int i = 0; i < fs.size(); i++)
		n += fs.at(i)->n; //Note that you have to have calculated face normals
	n[0] /= fs.size();
	n[1] /= fs.size();
	n[2] /= fs.size();

	n.Normalize();
	return n;
}

Vector3D Mesh::get_normal_to_face(face *f){
	Vector3D n = create_Vector3D(0,0,0);
	vertex *v = f->edges.at(0)->v1;
	vertex *v1 = f->edges.at(0)->v2;
	vertex *v2 = NULL;
	for(int i = 1;v2 == NULL && i < f->edges.size(); i++){
		if(f->edges.at(i)->v1->id != v->id && f->edges.at(i)->v1->id != v1->id)
			v2 = f->edges.at(i)->v1;
		else if(f->edges.at(i)->v2->id != v->id && f->edges.at(i)->v2->id != v1->id)
			v2 = f->edges.at(i)->v2;
	}
	Vector3D a = v1->pos - v->pos;
	Vector3D b = v2->pos - v->pos;
	a.Normalize();
	b.Normalize();
	n[0] = a[1]*b[2]-a[2]*b[1];
	n[1] = a[2]*b[0]-a[0]*b[2];
	n[2] = a[0]*b[1]-a[1]*b[0];

	cout << "a: " << a << "  b: " << b << "  n: " << n << endl; 
	return n;
}
float Mesh::get_max_dist_to_neighbor(vertex *v){
	float max = 0;
	float val;
	for(int i = 0;  i< v->neighbors.size(); i++){
		val = (v->neighbors.at(i)->pos-v->pos).GetNorm();
		max = val > max ? val : max;
	}
	return max;
}

edge* Mesh::edge_exists(int a, int b){
	for(int i = 0; i < edges.size(); i++){
		if(edges.at(i)->v1->id == a || edges.at(i)->v1->id == b){
			if(edges.at(i)->v2->id == a || edges.at(i)->v2->id == b)
				return edges.at(i);
		}
	}
	return NULL;
}

int Mesh::parse_file(string f){
	vector<Vector3D> nodes;
	int vertex_, faces_, edges_, nr, i, j;
	float x, y, z;
	int coff, noff, off, fouroff, nothing;
	char first, second;
	char dummy[256];
	ifstream file;
	coff = noff = off = fouroff = 0;
	file.open(f.c_str(), ifstream::in);
	cout << "initiating mesh" << endl;
	if(file.fail()){
		cout << "could not open " << f << endl;
		return 1;
	}

	file >> first;
	file >> second;

	if(first == 'O' || first == 'o'){
		off = 1;
		//läs bort raden
		file.getline(dummy, 256);
	}else if(second == 'O' || second == 'o'){
		if(first == 'C' || first == 'c')
			coff = 1;
		else if(first == 'N' || first == 'n')
			noff = 1;
		else if(first == '4')
			fouroff = 1;
		//läs bort raden
		file.getline(dummy, 256);
	}else{
		nothing = 1;
		vertex_ = first;
		faces_ = second;
		file >> edges_;
	}

	file >> vertex_;
	file >> faces_;
	file >> edges_;
	
	//läs in noder
	for(i = 0; i<vertex_; i++){
		file >> x;
		file >> y;
		file >> z;
		Vector3D p = create_Vector3D(x,y,z);
		vertex *v = new vertex;
		v->id = i;
		v->pos = p;
		vertices.push_back(v);
		//om det kommer en kommentar läs bort raden
		file.ignore(256, '\n');
	}
	id = i;
	cout << "vertices in file: " << vertex_ << endl;
	//läs in faces
	int temp;
	vector<int> poly;
	
	for(i = 0; i<faces_; i++){
		file >> nr;
		for(j = 0; j<nr; j++){
			//lagra värdena här i face
			file >> temp;
//			cout << temp << " added\n";
			poly.push_back(temp);
		}
		file.ignore(256, '\n');
		face *f = new face;
		for(int a = 0; a < poly.size(); a++){
			
			edge *e;
			if((e =edge_exists(poly.at(a),poly.at((a+1)%poly.size()))) == NULL){
				e = new edge;
				e->middle = -1;
				e->f1 = f;
				e->f2 = NULL;
				e->v1 = vertices.at(poly.at(a));
				e->v2 = vertices.at(poly.at((a+1)%poly.size()));

				e->v1->neighbors.push_back(e->v2);
				e->v2->neighbors.push_back(e->v1);

				edges.push_back(e);
			}else{
				e->f2 = f;
			}
			f->edges.push_back(e);
		}
		faces.push_back(f);
		//add_polygon(poly);
		poly.clear();
		
	}
	//läs in edges... Verkar som det inte är några

	file.close();

	return 0;
}
