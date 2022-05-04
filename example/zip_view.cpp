
#include <iris/ranges/view/zip_view.hpp>

#include <forward_list>
#include <iostream>
#include <list>
#include <vector>

using namespace iris;

int main()
{
    std::vector v = { 1, 2 };
    std::list l = { 'a', 'b', 'c' };
    std::forward_list fl = { 10ll, 20ll, 30ll, 40ll };

    for (auto&& [x, y, z] : views::zip(v, l, fl)) {
        std::cout << '(' << x << ", " << y << ", " << z << ")\n";
    }
    // (1, a, 10)
    // (2, b, 20)

    return 0;
}
