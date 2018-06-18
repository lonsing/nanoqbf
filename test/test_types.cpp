//
// Created by vedad on 18/06/18.
//

#include "../Logger.h"
#include "../types.h"
#include <vector>
#include <iostream>
#include <random>
#include <assert.h>

void test_make_assignment()
{
  std::vector<Lit> my_lits;
  std::random_device dev;
  std::mt19937 rand(dev());
  
  for(int i = 0; i < 100; i++)
  {
    int n = 1 + (unsigned)rand() % 40;
    std::cout << i << " " << n << " : ";
    my_lits.clear();
    for(int j = 1; j <= n; j++)
    {
      my_lits.push_back(rand() % 2 ? j : -j);
      std::cout << my_lits.back() << " ";
    }
    std::cout << std::endl;
    
    Assignment* a = Assignment::make_assignemnt(my_lits);
    assert(a->size == my_lits.size());
    for(int j = 0; j < n; j++)
      assert(a->get(j) != sign(my_lits[j]));
    int m = 100 + (unsigned)rand() % 800;
    for(int j = 0; j < m; j++)
    {
      int p = (unsigned)rand() % n;
      bool s = (bool)((unsigned)rand() % 2);
      a->set(p, s);
      assert(a->get(p) == s);
    }
    
    Assignment::destroy_assignemnt(a);
  }
}


int main()
{
  LOG("Hello there, this is the testing file");
  
  test_make_assignment();
  
  return 0;
}