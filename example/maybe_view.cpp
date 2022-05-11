
#include <iris/ranges/view/maybe_view.hpp>

#include <iostream>
#include <optional>
#include <vector>

using namespace iris;

int main()
{
    std::vector<std::optional<int>> v { std::optional<int> { 42 },
                                        std::optional<int> {},
                                        std::optional<int> { 6 * 9 } };

    auto r = std::views::join(std::views::transform(v, views::maybe));

    for (auto i : r) {
        std::cout << i << "\n";
    }
    // 42
    // 54

    return 0;
}
