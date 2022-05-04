
#include <iris/ranges/view/chunk_view.hpp>

#include <iostream>
#include <vector>

using namespace iris;

int main()
{
    std::vector v = { 0, 1, 2, 3, 4, 5, 6 };

    for (auto r : v | views::chunk(3)) {
        std::cout << '[';
        auto sep = "";
        for (auto i : r) {
            std::cout << sep << i;
            sep = ", ";
        }
        std::cout << "] ";
    }
    // [0, 1, 2] [3, 4, 5] [6]

    return 0;
}
