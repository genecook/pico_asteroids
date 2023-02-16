#include <stdio.h>
#include <iostream>
#include <string>

#include <ast_obj.h>

int main() {
  std::cout << "Test astObj class..." << std::endl;

  std::vector<struct coordinate> my_outline;

  // this square is all in...

  my_outline.push_back(coordinate(0,0));
  my_outline.push_back(coordinate(100,0));
  my_outline.push_back(coordinate(100,100));
  my_outline.push_back(coordinate(0,100));
  my_outline.push_back(coordinate(0,0));

  class astObj my_astobj(&my_outline,0,0);

  my_astobj.SetTrajectory(10, 10);

  my_astobj.Advance();
  my_astobj.DumpLineSegments();
  std::cout << std::endl;

  my_astobj.Advance();
  my_astobj.DumpLineSegments();
  std::cout << std::endl;

  my_astobj.Advance();
  my_astobj.DumpLineSegments();
  std::cout << std::endl;
  
  return 0;
}

