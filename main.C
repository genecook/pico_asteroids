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

  std::cout << "On screen instance. move it a few times within screen..." << std::endl;
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
  
  std::cout << "Off screen instance. better not show up..." << std::endl;

  class astObj my_astobj2(&my_outline,-100,-100);
  my_astobj2.Advance();
  my_astobj2.DumpLineSegments();
  std::cout << std::endl;

  std::cout << "Now set trajectory to advance this hidden instance onto screen..." << std::endl;

  my_astobj2.SetTrajectory(100, 100);
  my_astobj2.Advance();
  my_astobj2.DumpLineSegments();
  std::cout << std::endl;

  std::cout << "Now set trajectory to cause object to move left, partially off screen..." << std::endl;

  my_astobj2.SetTrajectory(-10, 0);
  my_astobj2.Advance();
  my_astobj2.DumpLineSegments();
  std::cout << std::endl;

  return 0;
}

