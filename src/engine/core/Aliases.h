#pragma once
#include <memory>
#include <vector>

template <typename T>
using Ref = std::shared_ptr<T>;
template <typename T>
using TempRef = std::weak_ptr<T>;
template <typename T>
using List = std::vector<T>;