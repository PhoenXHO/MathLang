#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <iterator>

template <typename It>
class MultiRangeIterator;

template <typename It>
class MultiRange
{
	std::vector<std::pair<It, It>> ranges;

public:
	MultiRange(std::vector<std::pair<It, It>> ranges) :
		ranges(ranges)
	{}
	MultiRange(It & begin, It & end)
	{
		add_range(std::make_pair(begin, end));
	}

	void add_range(std::pair<It, It> range)
	{
		if (range.first != range.second)
			ranges.push_back(range);
	}
	void add_range(MultiRange<It> range)
	{
		ranges.insert(ranges.end(), range.ranges.begin(), range.ranges.end());
	}

	bool empty() const { return ranges.empty(); }

	MultiRangeIterator<It> begin()
	{
		if (ranges.empty())
			return end();
		return MultiRangeIterator<It>(ranges.begin(), ranges.end());
	}
	MultiRangeIterator<It> end()
	{
		return MultiRangeIterator<It>(ranges.end(), ranges.end());
	}
};

template <typename It>
class MultiRangeIterator
{
	typename std::vector<std::pair<It, It>>::iterator current_range;
	typename std::vector<std::pair<It, It>>::iterator end_range;

public:
	MultiRangeIterator(
		typename std::vector<std::pair<It, It>>::iterator current_range,
		typename std::vector<std::pair<It, It>>::iterator end_range
	) :
		current_range(current_range),
		end_range(end_range)
	{}

	const auto & operator*() const { return *current_range->first; }
	auto & operator*() { return *current_range->first; }
	const auto * operator->() const { return &*current_range->first; }
	auto * operator->() { return &*current_range->first; }

	MultiRangeIterator & operator++()
	{
		++current_range->first;
		if (current_range->first == current_range->second && current_range != end_range)
			++current_range;
		return *this;
	}

	bool operator==(const MultiRangeIterator & other) const
	{
		return current_range == other.current_range;
	}
	bool operator!=(const MultiRangeIterator & other) const
	{
		return !(*this == other);
	}
};

#endif // UTIL_H