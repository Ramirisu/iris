
#include <iris/ranges/view/zip_transform_view.hpp>

#include <iostream>
#include <vector>

using namespace iris;

int main()
{
    std::vector v1 = { 1, 2 };
    std::vector v2 = { 4, 5, 6 };

    for (auto i : views::zip_transform(std::plus(), v1, v2)) {
        std::cout << i << "\n";
    }
    // 5
    // 7

    return 0;
}
