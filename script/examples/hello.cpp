/*
 * SimpleOS script/examples - C++ örneği
 *
 * Derleme (WSL/Linux):
 *   g++ -std=c++17 -Wall -Wextra -O2 hello.cpp -o hello_cpp
 */

#include <iostream>
#include <string>

static std::string ipv4_format(unsigned a, unsigned b, unsigned c, unsigned d)
{
  return std::to_string(a) + "." + std::to_string(b) + "." + std::to_string(c) + "." + std::to_string(d);
}

int main()
{
  std::cout << "SimpleOS C++ örnek: " << ipv4_format(127, 0, 0, 1) << "\n";
  return 0;
}

