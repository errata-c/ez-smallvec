#include <catch2/catch_all.hpp>
#include <ez/smallvec.hpp>

#include <memory>
#include <vector>

TEST_CASE("simple int test") {
	ez::smallvec<int, 4> vec;

	REQUIRE(vec.size() == 0);
	REQUIRE(vec.max_size() == 4);
	REQUIRE(vec.capacity() == 4);
	REQUIRE(vec.empty());

	vec.push_back(0);
	REQUIRE(vec.size() == 1);
	REQUIRE(!vec.empty());
	REQUIRE(vec[0] == 0);
	REQUIRE(vec.at(0) == 0);

	vec.emplace_back(1);
	REQUIRE(vec.size() == 2);
	REQUIRE(vec[0] == 0);
	REQUIRE(vec[1] == 1);

	REQUIRE(vec.at(0) == 0);
	REQUIRE(vec.at(1) == 1);

	int* data = vec.data();
	REQUIRE(data[0] == 0);
	REQUIRE(data[1] == 1);

	vec.push_back(2);
	vec.push_back(3);

	REQUIRE(vec.size() == vec.max_size());

	vec.clear();
	REQUIRE(vec.empty());
	REQUIRE(vec.size() == 0);

	vec.assign({0,1,2,3});

	REQUIRE(vec.size() == 4);
	for (int i = 0; i < 4; ++i) {
		CAPTURE(i);
		REQUIRE(vec[i] == i);
	}

	REQUIRE(vec.front() == 0);
	REQUIRE(vec.back() == 3);
	
	vec.clear();
	vec.assign(4, 0);

	for (int i = 0; i < 4; ++i) {
		CAPTURE(i);
		REQUIRE(vec[i] == 0);
	}

	std::vector<int> tmp{0,1,2,3};

	vec.assign(tmp.begin(), tmp.end());

	for (int i = 0; i < 4; ++i) {
		CAPTURE(i);
		REQUIRE(vec[i] == i);
	}

	auto it = vec.erase(vec.begin());
	REQUIRE(vec.size() == 3);
	REQUIRE(vec[0] == 1);
	REQUIRE(vec[1] == 2);
	REQUIRE(vec[2] == 3);

	REQUIRE(it == vec.begin());

	it = vec.erase(vec.begin(), vec.begin() + 3);
	REQUIRE(vec.size() == 0);
	REQUIRE(it == vec.begin());
	REQUIRE(it == vec.end());
}

TEST_CASE("string test") {
	std::weak_ptr<std::string> tracker;
	ez::smallvec<std::shared_ptr<std::string>, 16> vec;
	{
		std::shared_ptr<std::string> str = std::make_shared<std::string>("Testing a string");
		tracker = str;
		vec.push_back(std::move(str));
	}

	REQUIRE(vec.size() == 1);
	REQUIRE(!vec.empty());

	REQUIRE(tracker.use_count() == 1);
	REQUIRE(!tracker.expired());

	vec.push_back(tracker.lock());
	REQUIRE(vec.size() == 2);
	REQUIRE(tracker.use_count() == 2);

	vec.push_back(tracker.lock());
	REQUIRE(vec.size() == 3);
	REQUIRE(tracker.use_count() == 3);

	vec.pop_back();
	REQUIRE(vec.size() == 2);
	REQUIRE(tracker.use_count() == 2);

	vec.pop_back();
	REQUIRE(vec.size() == 1);
	REQUIRE(tracker.use_count() == 1);

	vec.pop_back();
	REQUIRE(vec.size() == 0);
	REQUIRE(tracker.expired());
}