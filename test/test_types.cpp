//
// Created by vedad on 18/06/18.
//

#include "../Logger.h"
#include "../types/common.h"
#include "../types/Assignment.h"
#include <vector>
#include <iostream>
#include <random>
#include <assert.h>
#include <cstring>
#include "../hashes.h"

void test_make_assignment()
{
  std::vector<Lit> my_lits;
  std::random_device dev;
  std::mt19937 rand(dev());
  
  for(int i = 0; i < 10; i++)
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
    
    Assignment* a = Assignment::make_assignment(my_lits);
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
    
    Assignment::destroy_assignment(a);
  }
  
  for(int i = 0; i < 10; i++)
  {
    int n = 40 + (unsigned)rand() % 160;
    std::cout << i << " " << n << " : ";
    my_lits.clear();
    for(int j = 1; j <= n; j++)
    {
      my_lits.push_back(rand() % 2 ? j : -j);
      std::cout << my_lits.back() << " ";
    }
    std::cout << std::endl;
    
    Assignment* a = Assignment::make_assignment(my_lits);
    Assignment* b = Assignment::make_assignment(n);
    for(unsigned j = 0; j < (unsigned)n; j++)
    {
      std::cout << "writing " << j << std::endl << std::flush;
      b->set(j, !sign(my_lits[j]));
      std::cout << "reading " << j << std::endl << std::flush;
      assert(b->get(j) == !sign(my_lits[j]));
    }
      
    b->rehash();
  
    assert(a->hash_value == a->hash_value);
    assert(a->size == a->size);
    assert(std::memcmp(a->bits, a->bits, (a->size + 7) / 8) == 0);
    assert(*a == *a);
    
    assert(b->hash_value == b->hash_value);
    assert(b->size == b->size);
    assert(std::memcmp(b->bits, b->bits, (b->size + 7) / 8) == 0);
    assert(*b == *b);
  
    assert(a->hash_value == b->hash_value);
    assert(a->size == b->size);
    assert(std::memcmp(a->bits, b->bits, (a->size + 7) / 8) == 0);
    assert(*a == *b);
  
  
    Assignment::destroy_assignment(a);
    Assignment::destroy_assignment(b);
  }
  
  
}

void test_copy_assignment()
{
  std::vector<Lit> my_lits;
  std::random_device dev;
  std::mt19937 rand(dev());
  
  int n = 1 + (unsigned)rand() % 40;
  Assignment* a = Assignment::make_assignment(n);
  
  for(int i = 0; i < 10; i++)
  {
    std::cout << i << " " << n << " : ";
    my_lits.clear();
    for(int j = 1; j <= n; j++)
    {
      my_lits.push_back(rand() % 2 ? j : -j);
      std::cout << my_lits.back() << " ";
    }
    std::cout << std::endl;
    
    a->update(my_lits);
    a->rehash();
    Assignment* b = Assignment::copy_assignment(a);
    CompAssignment cmp;
    assert(a->hash_value == b->hash_value);
    assert(a->size == b->size);
    assert(std::memcmp(a->bits, b->bits, (a->size + 7) / 8) == 0);
    assert(cmp(a, b));
    
    Assignment::destroy_assignment(b);
  }
  Assignment::destroy_assignment(a);
}

void test_hash_assignment()
{
  std::vector<Lit> my_lits;
  std::random_device dev;
  std::mt19937 rand(dev());
  
  for (int i = 0; i < 9; i++)
    my_lits.push_back(rand() % 2 ? i + 1 : -(i + 1));
  Assignment* a = Assignment::make_assignment(my_lits);
  
  std::cout << "a : " << a->hash_value << " : ";
  for (int i = 0; i < 9; i++)
    std::cout << my_lits[i] << " ";
  std::cout << std::endl;
  
  for(int i = 9; i < 14; i++)
    my_lits.push_back(-(i + 1));
  Assignment* b = Assignment::make_assignment(my_lits);
  
  std::cout << "b : " << b->hash_value << " : ";
  for (int i = 0; i < 14; i++)
    std::cout << my_lits[i] << " ";
  std::cout << std::endl;
  
  assert(a->hash_value != b->hash_value);
  
  Assignment::destroy_assignment(a);
  Assignment::destroy_assignment(b);
}

void test_make_lit()
{
  std::vector<Lit> my_lits;
  std::random_device dev;
  std::mt19937 rand(dev());
  
  for(int i = 0; i < 100; i++)
  {
    Var v = 1 + (unsigned)rand() % 1000;
    assert(make_lit(v, 0) == (Lit)v);
    assert(make_lit(v, 1) == -((Lit)v));
    std::cout << make_lit(v, 0) << " " << make_lit(v, 1) << std::endl;
  }
}


int main()
{
  LOG("Hello there, this is the testing file");
  
  test_make_assignment();
  test_copy_assignment();
  test_hash_assignment();
  test_make_lit();
  
  return 0;
}