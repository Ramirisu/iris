
#include <iris/ranges/view/adjacent_transform_view.hpp>

#include <iostream>
#include <vector>

using namespace iris;

struct multiplies {
    template <typename T>
    T operator()(T a, T b, T c) const
    {
        return a * b * c;
    }
};

int main()
{
    std::vector v = { 1, 2, 3, 4, 5 };

    for (auto i : v | views::adjacent_transform<3>(multiplies())) {
        std::cout << i << "\n";
    }
    // 6
    // 24
    // 60

    return 0;
}
