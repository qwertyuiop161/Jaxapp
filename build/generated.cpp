#include <iostream>
#include <string>


int main()
{
    int age = 14;
    bool teen = ((age >= 13) && (age < 18));
    bool notTeen = (!teen);
    bool allowed = (teen || notTeen);
    std::cout << teen << '\n';
    std::cout << notTeen << '\n';
    std::cout << allowed << '\n';
    return 0;
}

