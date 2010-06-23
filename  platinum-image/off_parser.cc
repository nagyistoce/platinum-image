#include "off_parser.h"

using namespace std;


int off_parser::parse_file(string f, Halfedge *fig){
	float min,max;
	int vertex, faces, edges, nr, i, j;
	float x, y, z;
	int coff, noff, off, fouroff, nothing;
	char first, second;
	char dummy[256];
	ifstream file;
	coff = noff = off = fouroff = 0;
	file.open(f.c_str(), ifstream::in);


	min = 100000;
	max = -min;
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
		vertex = first;
		faces = second;
		file >> edges;
	}

	file >> vertex;
	file >> faces;
	file >> edges;
	
//	cout << "v: " << vertex << "\nf: " << faces << "\ne: " << edges << endl;
	//fig = new Figure(vertex, faces, edges);
	//läs in noder
	for(i = 0; i<vertex; i++){
		file >> x;
		file >> y;
		file >> z;
		fig->addVertex(x, y, z);
		min = std::min(min,x);
		max = std::max(max,x);
//		cout << "adding vertex " << (i+1) << endl;
		//om det kommer en kommentar läs bort raden
			//if(file.peek() == '#'){
				file.ignore(256, '\n');
			/*	cout << "tog bort: " << dummy << endl;
			}*/
	}
	//läs in faces
	int temp;
	vector<int> face;
	
	for(i = 0; i<faces; i++){
		file >> nr;
		for(j = 0; j<nr; j++){
			//lagra värdena här i face
			file >> temp;
//			cout << temp << " added\n";
			face.push_back(temp);
		}
		file.ignore(256, '\n');
		fig->addPolygon(face);
		face.clear();
//		cout << "adding polygon " << (i+1) << endl;
		
	}
	//läs in edges... Verkar som det inte är några
	float diff = max-min;
	for(i=0; i<fig->vertices.size(); i++){
		VertexData *v = fig->vertices.at(i);
		v->x = ((v->x - min)/diff)*2.0 - 1.0;
		v->y = ((v->y - min)/diff)*2.0 - 1.0;
		v->z = ((v->z - min)/diff)*2.0 - 1.0;
	}
	file.close();

	return 0;
}

void off_parser::save_file(string f, Halfedge *fig){
	int i, j;
	std::vector<int> face;
	printf("Save OFF: \"%s\"\n", f.c_str());
	std::ofstream myfile;
	myfile.open (f.c_str());
	myfile << "OFF\n";
	myfile << fig->vertices.size() << " ";
	myfile << fig->polygons.size() << " ";
	myfile << fig->edges.size() << "\n";
	for(i = 0; i<fig->vertices.size(); i++){
		myfile << fig->vertices[i]->x << " ";
		myfile << fig->vertices[i]->y << " ";
		myfile << fig->vertices[i]->z << "\n";
	}
	for(i = 0; i<fig->polygons.size(); i++){
		HalfData* current;
		current = fig->polygons[i]->half;
		do{
			face.push_back(current->origin->index);
			current = current->next;
		}while(fig->polygons[i]->half != current);
		myfile << face.size() << " ";
		for(j = 0; j<face.size(); j++){
			myfile << face[j] << " ";
		}
		myfile << "\n";
		face.clear();
	}
	printf("Saved it :) \n");
	myfile.close();
}
