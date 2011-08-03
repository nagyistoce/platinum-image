#include "halfedge_ops.h"

void halfedge_ops::setNormals(PolygonData *start){
	float norm[3];
	float n;
	float x1, x2, y1, y2, z1, z2;
   HalfData* first;
	HalfData* second;
	HalfData* third;
   
	first = start->half;
	second = first->next;
	third = second->next;

	x1 = second->origin->x - first->origin->x;
	x2 = third->origin->x - first->origin->x;

	y1 = second->origin->y - first->origin->y;
	y2 = third->origin->y - first->origin->y;

	z1 = second->origin->z - first->origin->z;
	z2 = third->origin->z - first->origin->z;

	//nu ska man ta cross mellan dom.
      
	norm[0] = y1*z2 - z1*y2;
   norm[1] = z1*x2 - x1*z2;
   norm[2] = x1*y2 - y1*x2;

	//Normaliserar normalen också

	n = sqrt(pow(norm[0],2) + pow(norm[1],2) +pow(norm[2],2));

	start->nx = norm[0]/n;
	start->ny = norm[1]/n;
	start->nz = norm[2]/n;
	 
}

void halfedge_ops::setVertexNormal(Halfedge* h, Halfedge* newfig){
	int n;
	float N[3];
	int i;
	VertexData* temp;
	for(i = 0; i<newfig->vertices.size(); i++){
		if (newfig->vertices[i]->half != NULL){

			temp = h->vertices[i];
			HalfData* begin = temp->half->pair;//->origin->half;
			HalfData* half = begin;
			N[0] = N[1] = N[2] = 0;
			n = 0;
			do
			{
				n++;
				N[0]+=half->left->nx;
				N[1]+=half->left->ny;
				N[2]+=half->left->nz;
				//std::cout << "normals for [" << newfig->vertices[i]->x << " " << newfig->vertices[i]->y << " " << newfig->vertices[i]->z << "]:  [" << half->left->nx << " " << half->left->ny << " " << half->left->nz << "]" << std::endl; 
				half = half->next->pair;//->next;//->origin->half;
			}
			while(half != begin);
			N[0] = N[0]/n;
			N[1] = N[1]/n;
			N[2] = N[2]/n;
			float nor = sqrt(pow(N[0],2) + pow(N[1],2) +pow(N[2],2));

			newfig->vertices[i]->nx = N[0]/nor;
			newfig->vertices[i]->ny = N[1]/nor;
			newfig->vertices[i]->nz = N[2]/nor;
		}
	}
}

void halfedge_ops::getmidpoint(PolygonData *start){
	int count;
	float mid[3];
	HalfData* current;
	mid[0] = mid[1] = mid[2] = 0.0;
	count = 0;
	current = start->half;
	do{
		count++;
		mid[0] += current->origin->x;
		mid[1] += current->origin->y;
		mid[2] += current->origin->z;
		current = current->next;
	}while(start->half != current);
	start->x = mid[0]/count;
	start->y = mid[1]/count;
	start->z = mid[2]/count;

}
std::vector<int> halfedge_ops::getmidedgepoint(PolygonData *start, Halfedge *newfig){
	VertexData *to;
	VertexData *from;
	HalfData *current;
	int index;
	std::vector<int> vert;
	current = start->half;
	do{
		from = current->origin;
		to = current->pair->origin;
		
		current->edge->x = to->x + from->x;
		current->edge->y = to->y + from->y;
		current->edge->z = to->z + from->z;

		current->edge->x += current->left->x + current->pair->left->x;
      current->edge->y += current->left->y + current->pair->left->y;
      current->edge->z += current->left->z + current->pair->left->z;

		current->edge->x = current->edge->x / 4.0;
      current->edge->y = current->edge->y / 4.0;
      current->edge->z = current->edge->z / 4.0;
		
		from->old = 1;
		vert.push_back(from->index);

		//Skapa och lägg till den nya edgepointen
		index = newfig->addVertex(current->edge->x, current->edge->y, current->edge->z);
		newfig->vertices[index]->old = 0;
		vert.push_back(index);
	
		current = current->next;

	}while(start->half != current);

	return vert;
}

void halfedge_ops::catmull(Halfedge *f, Halfedge *newfig){
	//XXX Måste lagra hur många ursprungsvertex man har
	int i,s, index;
	std::vector<int> vert;
	for(i = 0; i<f->polygons.size(); i++){
		getmidpoint(f->polygons[i]);
	}
	for(i = 0; i<f->polygons.size(); i++){
		//Vissa räknas 2 ggr. Men men...
		vert = getmidedgepoint(f->polygons[i],newfig);
		
		//Lägg till den nya centerpointen
		index = newfig->addVertex(f->polygons[i]->x, f->polygons[i]->y, f->polygons[i]->z);
		newfig->vertices[index]->old = 0;
		//vert.push_back(newfig->vertices[index]);
		vert.push_back(index);
		newfig->subpolygon(vert);
		//newfig->addPolygon(vert);
		vert.clear();
	}
}
/*void halfedge_ops::calcNewVertex(Halfedge* h){
	int n;
	float F[3];
	float R[3];
	int i;
	VertexData* temp;
	for(i = 0; i<newfig->vertices.size(); i++){
		if (newfig->vertices[i]->half != NULL && newfig->vertices[i]->old){

			temp = h->vertices[i];
			HalfData* begin = temp->half->pair;//->origin->half;
			HalfData* half = begin;
			F[0] = F[1] = F[2] = 0;
			R[0] = R[1] = R[2] = 0;
			n = 0;
			do
			{
				n++;
				F[0]+=half->left->x;
				F[1]+=half->left->y;
				F[2]+=half->left->z;
				
				R[0]+=half->edge->x;
				R[1]+=half->edge->y;
				R[2]+=half->edge->z;
				
				half = half->next->pair;//->next;//->origin->half;
			}
			while(half != begin);
			F[0] = F[0]/n;
			F[1] = F[1]/n;
			F[2] = F[2]/n;
			
			R[0] = R[0]/n;
			R[1] = R[1]/n;
			R[2] = R[2]/n;

			newfig->vertices[i]->x = (F[0] + 2*R[0] + (n-3)*newfig->vertices[i]->x)/n;
			newfig->vertices[i]->y = (F[1] + 2*R[1] + (n-3)*newfig->vertices[i]->y)/n;
			newfig->vertices[i]->z = (F[2] + 2*R[2] + (n-3)*newfig->vertices[i]->z)/n;
		}else if(!newfig->vertices[i]->old){
			break;
		}
	}
}*/

Halfedge* halfedge_ops::subdivide(Halfedge *fig){
	int i, j, k;
	Halfedge *newfig = new Halfedge();
	for(j = 0; j<fig->vertices.size(); j++){
		newfig->addVertex(fig->vertices[j]->x,fig->vertices[j]->y,
			fig->vertices[j]->z);
		newfig->vertices[newfig->vertices.size()-1]->old = 1;
	}
	catmull(fig, newfig);
	//calcNewVertex(fig); DO not update positions (maybe)...
	//for(k = 0; k<newfig->polygons.size(); k++){
	//	setNormals(newfig->polygons[k]);
	//}
	//setVertexNormal(newfig);
	//fig = newfig;

	return newfig;
}

