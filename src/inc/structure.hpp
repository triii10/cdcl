#pragma once

#include <vector>
#include <unordered_map>
#include <type_traits>
#include <iostream>
#include <algorithm>
#include <cmath>


#define CLAUSE std::unordered_map<int, clauseInfo>
#define LITERAL std::unordered_map < int, std::vector <int> >

struct clauseInfo {
    bool unit;
    int unitLiteral;
    std::vector<int> clause;
};

enum state {
  UNDEF = -1, SAT = 1, UNSAT = 0
};

struct trailInfo {
    int decisionLevel;
    bool isDecisionLiteral;
    int impliedBy;
    std::vector<bool> assignments;
};

// by default you declare that no type is a container
template<typename T>
struct is_container {
  static constexpr bool value = false;
};

// then you specify which types are actually containers.. I'm just declaring
// std::vector<T, A>, but you could make a similar declaration for list, array,
// and so on.
template<typename T, typename Allocator>
struct is_container<std::vector<T, Allocator> > {
  static constexpr bool value = true;
};

template<typename T, typename Allocator>
struct is_container<std::unordered_map<T, Allocator> > {
  static constexpr bool value = false;
};

// then you enable a print function to print all elements in a container (you
// can modify the actual formatting to your liking)
template<typename T>
typename std::enable_if<
  is_container<T>::value,
  std::ostream& >::type
inline operator<<(std::ostream& os, const T& container) {
  const auto N = container.size();
  std::size_t current = 0;
  os << "[";
  for(const auto & item : container) {
    const char* separator = current++ == N - 1 ? "" : ", ";
    os << item << separator;
  }
  os << "]";
  return os;
}