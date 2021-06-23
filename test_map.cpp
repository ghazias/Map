#include <unordered_map>
#include <utility>
#include <vector>

#include "catch.hpp"
#include "map.hpp"
#include "map.hpp"  // include guard

template class dsc::Map<int, int>;  // explicit instantiation

TEST_CASE("Empty constructor") {
  dsc::Map<std::string, int> empty_map{};
  REQUIRE(empty_map.empty());
}

TEST_CASE("Copy constructor") {
  dsc::Map<int, char> original;  // initialize map to be copied
  std::vector<std::pair<int, char> > pairs{{5, 'e'}, {26, 'z'}, {1, 'a'}};

  for (auto p : pairs) {
    original.add(p);
  }

  dsc::Map<int, char> copy_of(original);

  SECTION("is deep copied and contains all elements") {
    for (auto p : pairs) {
      REQUIRE(&copy_of.get(p.first) != &original.get(p.first));
    }
  }

  SECTION("copy is independent of original") {
    copy_of.remove(pairs.front().first);

    REQUIRE_FALSE(copy_of.contains(pairs.front().first));
    REQUIRE(original.contains(pairs.front().first));
  }
}

TEST_CASE("Move constructor") {
  dsc::Map<int, float> original{};
  std::vector<std::pair<int, float> > pairs{{5, 5.5}, {2, 2.2}, {8, 8.8}};
  std::unordered_map<int, float*> addresses;

  for (const auto& p : pairs) {
    original.add(p);
    addresses[p.first] = &original[p.second];
  }

  dsc::Map<int, float> moved = std::move(original);

  SECTION("Element memory addresses are unchanged") {
    for (const auto& [key, addr] : addresses) {
      REQUIRE(&moved[key] == addr);
    }
  }

  // TODO add test for assigning over original map

  SECTION("Moved can be added to") {
    std::pair<int, float> added_pair = {9, 9.9};
    moved.add(added_pair);
    REQUIRE(moved.contains(added_pair.first));
  }

  SECTION("Source list is nulled") { REQUIRE(original.empty()); }
}

TEST_CASE("Copy assignment from an populated map") {
  dsc::Map<std::string, int> original{};
  std::vector<std::pair<std::string, int> > pairs{
      {"One", 1}, {"Two", 2}, {"Three", 3}};

  for (auto p : pairs) {
    original.add(p);
  }

  SECTION("Empty = original") {
    dsc::Map<std::string, int> copy_of = original;

    for (auto p : pairs) {
      REQUIRE(copy_of.contains(p.first));  // elements are present
      REQUIRE(&copy_of.get(p.first) !=
              &original.get(
                  p.first));  // element addresses are distinct from original
    }
  }

  SECTION("Nonempty = original") {
    dsc::Map<std::string, int> copy_of{};
    std::vector<std::pair<std::string, int> > other_pairs = {
        {"Four", 4}, {"Five", 5}, {"Six", 6}};

    for (auto p : other_pairs) {
      copy_of.add(p);
    }

    copy_of = original;

    for (auto p : pairs) {
      REQUIRE(copy_of.contains(p.first));
      REQUIRE(&copy_of.get(p.first) != &original.get(p.first));
    }
  }

  SECTION("original = original") {
    dsc::Map<std::string, int>* addr = &original;
    original = original;
    REQUIRE(&original == addr);
  }
}

TEST_CASE("Copy assignment from an empty map") {
  dsc::Map<int, int> original{};
  REQUIRE(original.empty());

  SECTION("Empty = original") {
    dsc::Map<int, int> copy_of = original;
    REQUIRE(copy_of.empty());
    // REQUIRE(&copy_of != original);
  }

  SECTION("Nonempty = original") {
    dsc::Map<int, int> copy_of{};
    std::vector<std::pair<int, int> > pairs{{1, 10}, {2, 20}, {3, 30}};

    for (auto p : pairs) {
      copy_of.add(p);
    }

    REQUIRE_FALSE(copy_of.empty());
    copy_of = original;

    REQUIRE(copy_of.empty());
    REQUIRE(&copy_of != &original);
  }

  SECTION("original = original") {
    dsc::Map<int, int>* addr = &original;
    original = original;
    REQUIRE(original.empty());   // still empty?
    REQUIRE(&original == addr);  // address of original has not changed
  }
}

TEST_CASE("Move assignment") {
  dsc::Map<int, int> original{};
  std::vector<std::pair<int, int> > pairs{{5, 50}, {6, 60}, {7, 70}};
  std::unordered_map<int, int*> addresses{};

  for (const auto& p : pairs) {
    original.add(p);
    addresses[p.first] = &original.get(p.first);
  }

  dsc::Map<int, int> moved = std::move(original);

  for (auto p : pairs) {
    REQUIRE(moved.contains(p.first));
    REQUIRE(&moved.get(p.first) == addresses[p.first]);
  }
}  // TODO

TEST_CASE("add()") {
  dsc::Map<char, char>
      opposite_chars{};  // TODO add initializer list constructor
  std::vector<std::pair<char, char> > pairs{{'a', 'z'}, {'b', 'y'}, {'c', 'x'}};

  for (auto p : pairs) {
    opposite_chars.add(p);
    REQUIRE(opposite_chars.contains(p.first));
  }
  // std::vector<std::pair<int, int> > pairs{{9, 50}, {6, 60}, {7, 70}};
}

TEST_CASE("contains()") {
  dsc::Map<int, std::string> map{};
  std::vector<std::pair<int, std::string> > pairs{
      {1, "One"}, {2, "Two"}, {3, "Three"}};

  for (auto p : pairs) {
    map.add(p);
    REQUIRE(map.contains(p.first));
  }

  REQUIRE_FALSE(map.contains(4));
}

TEST_CASE("get()") {
  dsc::Map<int, int> map{};
  std::vector<std::pair<int, int> > pairs{{5, 50}, {6, 60}, {7, 70}};

  for (auto p : pairs) {
    map.add(p);
    REQUIRE(map.get(p.first) == p.second);
  }

  REQUIRE(map.size() == pairs.size());
}

TEST_CASE("remove()") {
  dsc::Map<int, char> map{};
  std::vector<std::pair<int, char> > pairs{{5, 'e'}, {26, 'z'}, {1, 'a'}};

  for (auto p : pairs) {
    map.add(p);
  }

  SECTION("Stress test") {
    for (auto p : pairs) {
      REQUIRE(map.contains(p.first));
      map.remove(p.first);
      REQUIRE_FALSE(map.contains(p.first));
    }
  }

  SECTION("Remove root") {
    map.remove(pairs.front().first);

    REQUIRE(map.size() == pairs.size() - 1);
    REQUIRE_FALSE(map.contains(pairs.front().first));
  }
}

TEST_CASE("empty()") {
  dsc::Map<int, int> map{};

  REQUIRE(map.empty());

  map.add({1, 100});

  REQUIRE_FALSE(map.empty());
}
