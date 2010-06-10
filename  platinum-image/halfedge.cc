#include "halfedge.h"
Halfedge::Halfedge(){
	
}

Halfedge::~Halfedge(){
	int i;
	for(i = 0; i< polygons.size(); i++){
		free(polygons[i]);
	}
	for(i = 0; i<edges.size(); i++){
		free(edges[i]);
	}
	for(i = 0; i<vertices.size(); i++){
		free(vertices[i]);
	}
}

int Halfedge::addVertex(float x, float y, float z){
	int i;
	for(i = 0; i< vertices.size(); i++){
		if(x == vertices[i]->x && y == vertices[i]->y && z == vertices[i]->z){
			return i;
		}
	}
	VertexData *vertex = (VertexData*) malloc(sizeof(VertexData));
	vertex->half = NULL;
	vertex->x = x;
	vertex->y = y;
	vertex->z = z;
	vertex->old = false;
	vertex->index = vertices.size();
	vertices.push_back(vertex);
	return vertices.size() - 1;
}
EdgeData* Halfedge::addEdge(int fromV, int toV){

	if(fromV == toV){
		return NULL;
	}
	//XXX Kolla om det redan finns en edge här

	VertexData *fromVertex = vertices.at(fromV);
	VertexData *toVertex = vertices.at(toV);

	//Allocate an edge and two halfedges
	EdgeData *edge = (EdgeData*) malloc(sizeof(EdgeData));
	HalfData *from_to_half = (HalfData*) malloc(sizeof(HalfData));
	HalfData *to_from_half = (HalfData*) malloc(sizeof(HalfData));

	//Init halfedges and link them to each other
	edge->half = from_to_half;

	HalfData *halfData = from_to_half;

	halfData->next = to_from_half;
	halfData->previous = to_from_half;
	halfData->pair = to_from_half;
	halfData->origin = fromVertex;
	halfData->edge = edge;
	halfData->left = NULL;

	fromVertex->half = from_to_half;

	//En klar

	halfData = to_from_half;

	halfData->next = from_to_half;
	halfData->previous = from_to_half;
	halfData->pair = from_to_half;
	halfData->origin = toVertex;
	halfData->edge = edge;
	halfData->left = NULL;

	toVertex->half = to_from_half;
	
	//Två klara. Nu är halfedges länkade till varandra
	edges.push_back(edge);
	return edge;
}

HalfData* Halfedge::destination(HalfData* half){
	return half->pair->next;
}

bool Halfedge::isFree(HalfData* edge){
	//XXX XXX fixa denna
	
	return true;
}
HalfData* Halfedge::getEdge(int from, int to){
	int i;
	//Omvänd ordning är den som kan finnas
	VertexData *fromVertex = vertices.at(to);
   VertexData *toVertex = vertices.at(from);
	
	for(i = 0; i<edges.size(); i++){
		if(edges[i]->half->origin == fromVertex && 
				edges[i]->half->pair->origin == toVertex){
			//Är bara 2 som kan dela en edge.
			return edges[i]->half->pair;
		}
	}
	return addEdge(from, to)->half;
}
void Halfedge::addPolygon(std::vector<int> f){
	if(f.empty()){
		return;
	}
	int halfCount = f.size();
	int i, nextIndex;
	

	//Skapa edges och bind dom till polygonen
	for (i = 0;i < halfCount;i++)
	{
		nextIndex = i + 1;
		if (nextIndex == halfCount)
		{
			nextIndex = 0;
		}
		if(f[i] != f[nextIndex]){
			face.push_back(getEdge(f[i], f[nextIndex]));
		}
	}

	PolygonData *polygon = (PolygonData*) malloc(sizeof(PolygonData));
	polygon->half = face[0];

	halfCount = face.size();
	for(i = 0; i<halfCount; i++){
		nextIndex = i + 1;
      if (nextIndex == halfCount)
      {
         nextIndex = 0;
      }
		face[i]->left = polygon;
		face[i]->next = face[nextIndex];
		face[nextIndex]->previous = face[i];
	}
	face.clear();
	polygons.push_back(polygon);
}
void Halfedge::subpolygon(std::vector<int> all){

	//Lägg till allt 2 och 2 eller nåt;
	if(all.empty()){
		return;
	}
	int numPolygons = (all.size()) /2;
	int i,j, nextIndex;
	int allsize = all.size();
	std::vector<int> f;
	int start = 1;
	int halfCount;
	for(j=0; j<numPolygons; j++){
		
		f.push_back(all[allsize -1]);
		
		f.push_back(all[start%(allsize-1)]);
		
		start ++;
		f.push_back(all[start%(allsize-1)]);
		
		start ++;
		f.push_back(all[start%(allsize-1)]);
		
		//start ++;
		halfCount = f.size();
		//Skapa edges och bind dom till polygonen
		for (i = 0;i < halfCount;i++)
		{
			nextIndex = i + 1;
			if (nextIndex == halfCount)
			{
				nextIndex = 0;
			}
			if(f[i] != f[nextIndex]){
				face.push_back(getEdge(f[i], f[nextIndex]));
			}
		}

		PolygonData *polygon = (PolygonData*) malloc(sizeof(PolygonData));
		polygon->half = face[0];

		halfCount = face.size();
		for(i = 0; i<halfCount; i++){
			nextIndex = i + 1;
			if (nextIndex == halfCount)
			{
				nextIndex = 0;
			}
			face[i]->left = polygon;
			face[i]->next = face[nextIndex];
			face[nextIndex]->previous = face[i];
		}
		face.clear();
		polygons.push_back(polygon);
		f.clear();
	}
}

