#include <iostream>
#include <string>

int getNumber();

int getNumber()
{
    return 7;
}

int main()
{
    int x = getNumber();
    std::cout << x << '\n';
    return 0;
}

