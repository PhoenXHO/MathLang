#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

template <typename T>
class Registry
{
	static_assert(
		std::is_pointer<T>::value ||
		std::is_same<T, std::shared_ptr<typename T::element_type>>::value ||
		std::is_same<T, std::shared_ptr<T>>::value,
		"Registry can only store pointers or shared pointers"
	);

	std::unordered_map<std::string, size_t> indices;
	std::vector<T> objects;

public:
	Registry() = default;
	~Registry() = default;

	T define(std::string_view name, T object)
	{
		indices[std::string(name)] = objects.size();
		objects.push_back(object);
		return object;
	}

	size_t get_index(std::string_view name) const
	{
		auto it = indices.find(std::string(name));
		if (it != indices.end())
		{
			return it->second;
		}
		return -1;
	}
	/// @brief Get the object at the given index
	/// @return The object at the given index or `nullptr` if the index is out of bounds
	std::pair<size_t, T> find(std::string_view name) const
	{
		size_t index = get_index(name);
		if (index != -1)
		{
			return { index, objects[index] };
		}
		return { -1, nullptr };
	}

	size_t size() const
	{ return objects.size(); }

	/// @brief Get the object at the given index
	/// @return The object at the given index or `nullptr` if the index is out of bounds
	T operator[](size_t index) const
	{ return index < objects.size() ? objects[index] : nullptr; }

	/// @brief Get the object at the given index
	/// @return The object at the given index or `nullptr` if the index is out of bounds
	std::pair<size_t, T> operator[](std::string_view name) const
	{ return find(name); }

	std::vector<T>::iterator begin()
	{ return objects.begin(); }
	std::vector<T>::iterator end()
	{ return objects.end(); }
	std::vector<T>::const_iterator begin() const
	{ return objects.begin(); }
	std::vector<T>::const_iterator end() const
	{ return objects.end(); }
};