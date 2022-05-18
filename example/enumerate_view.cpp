
#include <iris/ranges/view/enumerate_view.hpp>

#include <iostream>

int main()
{
    static const int input[] = { 1, 2, 4, 8, 16 };

    // via enumerate_result
    for (auto&& e : iris::views::enumerate(input)) {
        std::cout << "index: " << e.index << ", value: " << e.value << "\n";
        // index: 0, value: 1
        // index: 1, value: 2
        // index: 2, value: 4
        // index: 3, value: 8
        // index: 4, value: 16
    }

    // via structured binding
    for (auto&& [index, value] : iris::views::enumerate(input)) {
        std::cout << "index: " << index << ", value: " << value << "\n";
        // index: 0, value: 1
        // index: 1, value: 2
        // index: 2, value: 4
        // index: 3, value: 8
        // index: 4, value: 16
    }

    return 0;
}
