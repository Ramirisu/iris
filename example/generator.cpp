
#include <iris/generator.hpp>
#include <iris/ranges.hpp>

#include <filesystem>
#include <iostream>

template <typename T>
iris::generator<const T&> fibonacci()
{
    T a = 0;
    T b = 1;
    co_yield a;
    co_yield b;
    for (;;) {
        T c = a + b;
        co_yield c;
        a = b;
        b = c;
    }
}

iris::generator<std::filesystem::path> list_dir(std::filesystem::path dir)
{
    if (!std::filesystem::is_directory(dir)) {
        co_return;
    }

    for (auto&& path : std::filesystem::directory_iterator(dir)) {
        co_yield path;
    }
}

iris::generator<std::filesystem::path>
list_dir_recursively(std::filesystem::path dir)
{
    if (!std::filesystem::is_directory(dir)) {
        co_return;
    }

    for (auto&& path : std::filesystem::directory_iterator(dir)) {
        if (std::filesystem::is_directory(path)) {
            co_yield iris::ranges::elements_of(list_dir_recursively(path));
        } else {
            co_yield path;
        }
    }

    // equivalent to:
    //
    // for (auto&& path : std::filesystem::recursive_directory_iterator(dir)) {
    //    if (!std::filesystem::is_directory(path)) {
    //        co_yield path;
    //    }
    // }
}

int main()
{
    {
        // print first 10 fibonacci numbers
        int count = 0;
        for (auto&& value : fibonacci<int>()) {
            std::cout << value << " ";
            if (++count == 10) {
                break;
            }
        }
        std::cout << "\n";
        // 0 1 1 2 3 5 8 13 21 34
    }
    {
        // iterate files non-recursivley
        for (auto&& path : list_dir("path/to/dir")) {
            std::cout << path << "\n";
        }
    }
    {
        // iterate files recursivley
        for (auto&& path : list_dir_recursively("path/to/dir")) {
            std::cout << path << "\n";
        }
    }

    return 0;
}
