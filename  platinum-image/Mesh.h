

//-------------------------------------------------------
#ifndef __Mesh__
#define __Mesh__

#include <cstdio>
#include <iostream>
#include <vector>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include "ptmath.h"
#include "image_binary.hxx"

enum MESH_SHAPE{M_CUBE,M_SPHERE};

using namespace std;

struct face;
struct edge;
struct vertex;

struct vertex{
	Vector3D pos;
	vector<vertex*> neighbors;
	int id;
};
struct edge{
	vertex* v1;
	vertex* v2;
	face* f1;
	face* f2;
	int middle;
};
struct face{
	vector<edge*> edges;
	Vector3D n; //Normal is good to store when vertex normal wants to be calculated later
};

class Mesh {
public:
	Mesh(MESH_SHAPE, int radius, int center);
	Mesh(image_binary<3> *start, int res);
	Mesh(string path, int radius, Vector3D center);
	void divide_all_polygons();
	vector<int> divide_polygon(face *f);
	void square_divide_all_polygons();
	void add_polygon(vector<Vector3D> v);
	Vector3D calc_mean_of_neighbors(vertex *v);

	vector<face*> get_all_faces_with_edges(vector<edge*> e);
	vector<edge*> get_all_edges_with_vertex_id(int id);
	Vector3D get_normal_to_vertex(vertex *v);
	Vector3D get_normal_to_face(face *f);
	float get_max_dist_to_neighbor(vertex *v);
	
	

	vector<face*> faces; //This needs to be accessed all over the place so...
	vector<vertex*> vertices;
	vector<edge*> edges;

private:
	void init_cube(int x1, int x2, int y1, int y2, int z1, int z2);
	void replace_polygon(face *f, vector<Vector3D> v);
	edge* does_edge_exist(Vector3D a, Vector3D b, vertex *a_ex, vertex *b_ex);
	edge* add_edge(Vector3D a, Vector3D b);
	vertex* find_next_vertex(face *f, int find, int no_find);
	void square_divide_polygon(face *f);
	void split_edges();

	edge* edge_exists(int a, int b);
	int parse_file(string f);

	int id;
};



#endif