#include <stdio.h>
#include <iostream>
#include <string>

#include <ast_obj.h>

int main() {
  std::cout << "Test astObj class..." << std::endl;

  class astObj my_astobj;

  std::vector<struct coordinate> my_outline;

  my_outline.push_back(coordinate(0,0));
  my_outline.push_back(coordinate(100,0));
  my_outline.push_back(coordinate(100,100));
  my_outline.push_back(coordinate(0,100));
  my_outline.push_back(coordinate(0,0));

  my_astobj.AddOutline(&my_outline);

  
  return 0;
}

