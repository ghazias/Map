#include <unordered_map>
#include <utility>
#include <vector>

#include "catch.hpp"
#include "map.hpp"
#include "map.hpp"  // include guard

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
      REQUIRE(&copy_of.get(p) != &original.get(p));
    }
  }

  SECTION("copy is independent of original") {
    copy_of.remove(pairs.front());

    REQUIRE_FALSE(copy_of.contains(pairs.front()));
    REQUIRE(original.contains(pairs.front()));
  }
}

TEST_CASE("Move constructor") {
  dsc::Map<int, float> original{};
  std::vector<std::pair<int, float> > pairs{{5, 5.5}, {2, 2.2}, {8, 8.8}};
  std::unordered_map<std::pair<std::pair<int, float>, std::pair<int, float>*> >
      addresses;  // TODO fix this

  for (auto p : pairs) {
    original.add(p);
    addresses[p] = &original.get(p);
  }

  dsc::Map<int, float> moved = std::move(original);

  SECTION("Element memory addresses are unchanged") {
    for (auto [p, addr] : addresses) {
      REQUIRE(
          &moved.get(p) ==
          addr);  // not entirely sure how or even if this line works, ask ryan
    }
  }

  SECTION("Moved map is assignable") {
    std::pair<int, float> added_pair = {9, 9.9};
    moved.add(added_pair);
    REQUIRE(moved.contains(added_pair));
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
      REQUIRE(copy_of.contains(p));  // elements are present
      REQUIRE(
          &copy_of.get(p) !=
          &original.get(p));  // element addresses are distinct from original
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
      REQUIRE(copy_of.contains(p));
      REQUIRE(&copy_of.get(p) != &original.get(p));
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
    REQUIRE(&copy_of != original);
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
  std::unordered_map<std::pair<int, int>, std::pair<int, int>*> addresses{};

  for (auto p : pairs) {
    original.add(p);
    addresses[p] = &original.get(p);
  }

  dsc::Map<int, int> moved = std::move(
      original);  // TODO how to actually do a move assignment vs constructor?

  for (auto p : pairs) {
    REQUIRE(moved.contains(p));
    REQUIRE(&moved.get(p) == addresses[v]);
  }
}

TEST_CASE("add()") {
  dsc::Map<char, char>
      opposite_chars{};  // TODO add initializer list constructor
  std::vector<std::pair<char, char> > pairs{{'a', 'z'}, {'b', 'y'}, {'c', 'x'}};

  for (auto p : pairs) {
    opposite_chars.add(p);
    REQUIRE(opposite_chars.contains(p));
  }
  std::vector<std::pair<int, int> > pairs{{5, 50}, {6, 60}, {7, 70}};
}

TEST_CASE("contains()") {
  dsc::Map<int, std::string> map{};
  std::vector<std::pair<int, std::string> > pairs{
      {1, "One"}, {2, "Two"}, {3, "Three"}};

  for (auto p : pairs) {
    map.add(p);
    REQUIRE(map.contains(p));
  }

  REQUIRE_FALSE(map.contains({4, "Four"}));
}

TEST_CASE("get()") {
  dsc::Map<int, int> map{};
  std::vector<std::pair<int, int> > pairs{{5, 50}, {6, 60}, {7, 70}};

  for (auto p : pairs) {
    map.add(p);
    REQUIRE(map.get(p) == p);
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
      REQUIRE(map.contains(p));
      map.remove(p);
      REQUIRE_FALSE(map.contains(p));
    }
  }

  SECTION("Remove root") {
    map.remove(pairs.front());

    REQUIRE(map.size() == pairs.size() - 1);
    REQUIRE_FALSE(map.contains(pairs.front()));
  }
}

TEST_CASE("empty()") {
  dsc::Map<int, int> map{};

  REQUIRE(map.empty());

  map.add({1, 100});

  REQUIRE_FALSE(map.empty());
}