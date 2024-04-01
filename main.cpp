#include <iostream>
#include <algorithm>
#include <random>
#include "cilk/cilk.h"
#include <ctime>
#include <chrono>

using namespace std;

const int BLOCK = 32;

void quicksort(int* l, int* r) {
    if (r - l < 2) {
        return;
    }
    r--;
    int *m = partition(l, r,
                       [pivot = *r](int a) { return a < pivot; });
    std::swap(*r, *m);
    quicksort(l, m);
    quicksort(m + 1, r + 1);
}

void par_quicksort(int* l, int* r) {
    if (r - l < BLOCK) {
        quicksort(l, r);
    } else {
        r--;
        int* m = partition(l, r,
                           [pivot=*r](int a) { return a < pivot; });
        std::swap(*r, *m);
        cilk_scope {
                cilk_spawn par_quicksort(l, m);
                par_quicksort(m+1, r+1);
        }
    }
}

int main() {
    int n = 100 * 1000 * 1000;

    default_random_engine rng;
    uniform_int_distribution<int> dist(0,n);
    vector<int> a(n);
    generate(a.begin(), a.end(), [&]() { return dist(rng); });

    cout << "Sorting " << n << " random integers\n";
    chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
    par_quicksort(a.data(), a.data() + a.size());
//    quicksort(a.data(), a.data() + a.size());
    chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();

    bool pass = is_sorted(a.cbegin(), a.cend());
    cout << "Sort " << ((pass) ? "succeeded" : "failed") << " in " << (end - start).count() / 1000000000.0 << " seconds\n";
    return 0;
}
