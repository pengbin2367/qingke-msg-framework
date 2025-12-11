find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(test_parse_cached test_parse_cached.cpp)
target_link_libraries(test_parse_cached msgbase GTest::GTest GTest::Main pthread)
add_test(NAME parse_cached_test COMMAND test_parse_cached)
