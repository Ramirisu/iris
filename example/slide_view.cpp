
#include <iris/ranges/view/slide_view.hpp>

#include <iostream>
#include <vector>

using namespace iris;

int main()
{
    std::vector v = { 0, 1, 2, 3, 4 };

    for (auto i : v | views::slide(3)) {
        std::cout << "[" << i[0] << ", " << i[1] << ", " << i[2] << "] ";
    }
    // [0, 1, 2] [1, 2, 3] [2, 3, 4]

    return 0;
}
