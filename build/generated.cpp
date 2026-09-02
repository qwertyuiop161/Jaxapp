#include <iostream>
#include <string>

void greet();

void greet(std::string name)
{
    std::cout << name << '\n';
}

int main()
{
    greet("Hashir");
    return 0;
}

