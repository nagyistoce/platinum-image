#ifndef OFF_PARSER__H
#define OFF_PARSER__H

#include <fstream>
#include <string>
#include <iostream>
#include "halfedge.h"

class off_parser{
   public:
      off_parser(){};
      ~off_parser(){};
      int parse_file(std::string s, Halfedge *fig);

};
#else
class Parser;
#endif
