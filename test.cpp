#include <iostream>
#include <vector>
#include <future>
#include <random>
#include <numeric>
#include <cassert>
#include "time_elapse.hpp"
static const int NUM = 100000000;
long long parallel_inner_product(std::vector<int>&v, std::vector<int>&w) {
    if (v.size() != w.size()) {
        return 0;
    }
    if (v.size() < 100) {
        return std::inner_product(begin(v), end(v), begin(w), 0LL);
    }

    auto future1 = std::async([&] { return std::inner_product(&v[0], &v[v.size() / 4], &w[0], 0LL); });
    auto future2 = std::async([&] { return std::inner_product(&v[v.size() / 4], &v[v.size() / 2], &w[v.size() / 4], 0LL); });
    auto future3 = std::async([&] { return std::inner_product(&v[v.size() / 2], &v[v.size() * 3 / 4], &w[v.size() / 2], 0LL); });
    auto future4 = std::async([&] { return std::inner_product(&v[v.size() * 3 / 4], &v[v.size()], &w[v.size() * 3 / 4], 0LL); });

    return future1.get() + future2.get() + future3.get() + future4.get();
}
long long serial_inner_product(std::vector<int>&v, std::vector<int>&w) {
    if (v.size() != w.size()) {
        return 0;
    }
    return std::inner_product(begin(v), end(v), begin(w), 0LL);
}
int main() {
    std::random_device seed;
    std::mt19937 engine(seed());
    std::uniform_int_distribution<int> dist(0, 100);
    std::vector<int> v, w;
    v.reserve(NUM);
    w.reserve(NUM);
    time_elapse te;
    te.start();
    for (int i = 0; i < NUM; ++i) {
        v.push_back(dist(engine));
        w.push_back(dist(engine));
    }
    std::cout << "load vector elapse = " << te.end() << "s" << std::endl;
    te.start();
    auto res1 = parallel_inner_product(v, w);
    std::cout << "parallel inner_product elapse = " << te.end() << "s" << std::endl;

    te.start();
    auto res2 = serial_inner_product(v, w);
    std::cout << "serial inner_product elapse = " << te.end() << "s" << std::endl;
 
    assert(res1 == res2);

    return 0;
}