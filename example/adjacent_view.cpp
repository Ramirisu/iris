
#include <iris/ranges/view/adjacent_view.hpp>

#include <iostream>
#include <vector>

using namespace iris;

int main()
{
    std::vector v = { 1, 2, 3, 4, 5 };

    for (auto [x, y, z] : v | views::adjacent<3>) {
        std::cout << "(" << x << ", " << y << ", " << z << ")\n";
    }
    // (1, 2, 3)
    // (2, 3, 4)
    // (3, 4, 5)

    return 0;
}
