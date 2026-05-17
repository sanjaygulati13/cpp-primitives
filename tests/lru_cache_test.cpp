// Tests for sgp::lru_cache.
// Tests double as documentation of the contract.

#include <sgp/lru_cache.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>

using sgp::lru_cache;

TEST_CASE("basic put / get", "[lru_cache]") {
    lru_cache<int, std::string> c{3};

    c.put(1, "one");
    c.put(2, "two");
    c.put(3, "three");

    REQUIRE(c.size() == 3);
    REQUIRE(c.get(1) == "one");
    REQUIRE(c.get(2) == "two");
    REQUIRE(c.get(3) == "three");
}

TEST_CASE("get on missing key returns nullopt", "[lru_cache]") {
    lru_cache<int, int> c{2};
    REQUIRE_FALSE(c.get(42).has_value());
}

TEST_CASE("eviction picks the least-recently-used", "[lru_cache]") {
    lru_cache<int, std::string> c{3};
    c.put(1, "one");
    c.put(2, "two");
    c.put(3, "three");

    // Access 1, promoting it to most-recent. Order: 1 (mru) <- 3 <- 2 (lru)
    REQUIRE(c.get(1) == "one");

    c.put(4, "four");  // evicts 2

    REQUIRE_FALSE(c.contains(2));
    REQUIRE(c.contains(1));
    REQUIRE(c.contains(3));
    REQUIRE(c.contains(4));
    REQUIRE(c.size() == 3);
}

TEST_CASE("put on existing key updates value and promotes", "[lru_cache]") {
    lru_cache<int, std::string> c{2};
    c.put(1, "one");
    c.put(2, "two");

    c.put(1, "ONE");  // update + promote
    c.put(3, "three");  // evicts 2 (now LRU), not 1

    REQUIRE(c.contains(1));
    REQUIRE_FALSE(c.contains(2));
    REQUIRE(c.contains(3));
    REQUIRE(c.get(1) == "ONE");
}

TEST_CASE("erase removes a key", "[lru_cache]") {
    lru_cache<int, int> c{3};
    c.put(1, 10);
    c.put(2, 20);

    REQUIRE(c.erase(1));
    REQUIRE_FALSE(c.contains(1));
    REQUIRE(c.size() == 1);
    REQUIRE_FALSE(c.erase(99));
}

TEST_CASE("peek does not promote", "[lru_cache]") {
    lru_cache<int, std::string> c{2};
    c.put(1, "one");
    c.put(2, "two");

    REQUIRE(c.peek(1) == "one");  // peek does NOT promote 1

    c.put(3, "three");  // evicts 1 (still LRU because peek didn't promote)

    REQUIRE_FALSE(c.contains(1));
    REQUIRE(c.contains(2));
    REQUIRE(c.contains(3));
}

TEST_CASE("clear resets the cache", "[lru_cache]") {
    lru_cache<int, int> c{3};
    c.put(1, 1);
    c.put(2, 2);

    c.clear();

    REQUIRE(c.empty());
    REQUIRE(c.size() == 0);
    REQUIRE_FALSE(c.contains(1));
}

TEST_CASE("works with string keys", "[lru_cache]") {
    lru_cache<std::string, int> c{2};
    c.put("alice", 1);
    c.put("bob", 2);

    REQUIRE(c.get("alice") == 1);
    c.put("carol", 3);  // evicts bob (LRU after alice was promoted)
    REQUIRE_FALSE(c.contains("bob"));
}

TEST_CASE("capacity of 1 always evicts on second insertion", "[lru_cache]") {
    lru_cache<int, int> c{1};
    c.put(1, 10);
    c.put(2, 20);
    REQUIRE_FALSE(c.contains(1));
    REQUIRE(c.get(2) == 20);
}

TEST_CASE("ordering survives many operations", "[lru_cache]") {
    lru_cache<int, int> c{5};
    for (int i = 0; i < 100; ++i) {
        c.put(i, i * 10);
    }
    // Only the last 5 inserts should remain
    REQUIRE(c.size() == 5);
    for (int i = 95; i < 100; ++i) {
        REQUIRE(c.get(i) == i * 10);
    }
    for (int i = 0; i < 95; ++i) {
        REQUIRE_FALSE(c.contains(i));
    }
}
