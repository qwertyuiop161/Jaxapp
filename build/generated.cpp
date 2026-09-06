#include <iostream>
#include <string>

int add(int a, int b);

int add(int a, int b)
{
    return (a + b);
}

int main()
{
    std::cout << add(5, 7) << '\n';
    return 0;
}

