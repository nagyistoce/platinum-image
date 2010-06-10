#ifndef HALFEDGE__H
#define HALFEDGE__H
#include <vector>
#include <list>
#include <string>
#include <iostream>
#include <fstream>

struct VertexData;
struct EdgeData;
struct PolygonData;
struct HalfData
{
   HalfData* next;
   HalfData* previous;
   HalfData* pair;

   VertexData* origin;
   PolygonData* left;
   EdgeData* edge;
};

struct VertexData
{
   float x, y, z;
   float nx, ny, nz;
   int index;
   bool old;
   HalfData* half;
};

struct EdgeData
{
   float x, y, z;
   HalfData* half;
};

struct PolygonData
{
   float x, y, z;
   float nx, ny, nz;
   HalfData* half;
};
class Halfedge{
   public:
      Halfedge();
      ~Halfedge();
      int addVertex(float x, float y, float z);
      EdgeData* addEdge(int fromV, int toV);
      void addPolygon(std::vector<int> face);
      void subpolygon(std::vector<int> ver);
      std::vector<EdgeData*> edges;
      std::vector<PolygonData*> polygons;
      std::vector<VertexData*> vertices; //lagra alla vertex här
      std::vector<PolygonData*> getPolygons(){
         return polygons;
      };
   private:
      std::vector<HalfData*> face;
      HalfData* destination(HalfData* edge);
      bool isFree(HalfData* edge);
      HalfData* getEdge(int to, int from);
};
#else
class Halfedge;
#endif
