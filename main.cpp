/* Driver program to test AVL map */
#include <bits/stdc++.h>

#include "avlmap.hpp"

using namespace std;

int main()
{
    Homebrew::AvlMap<int,int> a {{1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0},
                                 {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10,0}};
                                 
    Homebrew::AvlMap<int,int> b {a};
    auto c = a;
    auto d = std::move(b);
    Homebrew::AvlMap<int,int> e {std::move(a)};
    e.erase(8);
    e.erase(10);
    e.print();
    e[5] = 200;
    e.print();
    e[100] = 32;
    e.print();
    cout << e[999] << endl;
    e.print();
    e[999] += 1;
    cout << e.at(999) << endl;
    e[7] = 7*7*7*7;
    e[9] += e[7];
    e.print();
    if (e[12]) cout << "found"; // won't print
    if (e[12]) cout << "found";
    
    e.print();
    e.insert(9,0);
    e.print();
    
    for (int i=0; i< 1000000; ++i) {
        e.insert(i, i);
    } 
    
    return 0;
}