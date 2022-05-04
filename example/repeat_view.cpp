
#include <iris/ranges/view/repeat_view.hpp>

#include <iostream>

using namespace iris;

int main()
{
    for (int i : views::repeat(17, 4)) {
        std::cout << i << ' ';
    }
    // 17 17 17 17

    return 0;
}
