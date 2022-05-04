
#include <iris/ranges/view/chunk_by_view.hpp>

#include <iostream>
#include <vector>

using namespace iris;

int main()
{
    std::vector v = { 1, 2, 2, 3, 0, 4, 5, 2 };

    for (auto r : v | views::chunk_by(std::ranges::less_equal {})) {
        std::cout << "[";
        auto sep = "";
        for (auto i : r) {
            std::cout << sep << i;
            sep = ", ";
        }
        std::cout << "] ";
    }
    // [1, 2, 2, 3] [0, 4, 5] [2]

    return 0;
}
