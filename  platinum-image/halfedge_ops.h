#ifndef HALFEDGE_OPS__H
#define HALFEDGE_OPS__H
#include <vector>
#include <math.h>
#include "halfedge.h"

class halfedge_ops{
public:
	halfedge_ops(){};
	~halfedge_ops(){};
	void setNormals(PolygonData *start);
	void setVertexNormal(Halfedge* h, Halfedge* newfig);
	void getmidpoint(PolygonData *start);
	std::vector<int> getmidedgepoint(PolygonData *start, Halfedge *newfig);
	void catmull(Halfedge *f, Halfedge *newfig);
	Halfedge* subdivide(Halfedge *fig);
};
#else
class halfedge_ops;
#endif