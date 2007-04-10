

//

// Anv�nd en 1 bitsvolym till bin�ra data.

// n=1 maskas av en 8/(8/n) stor volym

//


#include "volumehandler_base.h"


// Denna �r INTE en template - finns bara en bin�rtyp - det speciella �r avkodningen

class volumehandler3Dbinary : public volumehandler_base // We MUST have a virtual base class

{

private:

 // vi vill anv�nda 1 bit per voxel, allts� dela l�ngd med 8

 unsigned char *volumeptr; // we will get pointer from ITK? Or internally?

 int width, height, depth; // volume size

// ??? int offsetX, offsetY, offsetZ;  // (beh�vs �ven offsets? Beror p� hur ITK lagrar volymsdata) - En volym kanske inte t�cker hela av en annan volym


public:

 void initialize_dataset(unsigned char *ptr, int w, int h, int d); // load must happen outside class - only part of the wholeness

// void raytrace(vector3D direction, int threshold, vector3D &resultpos, vector3D startpos = 0); // mostly a demo app.

};

