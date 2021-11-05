#include <iostream>
#include <chrono>
#include <limits>
#include <functional>
#include <assert.h>

#include "v8-instance-db.h"

struct test_sum_t {
    explicit test_sum_t(int a_, int b_)
    : a(a_),
      b(b_) {}

    int a;
    int b;

    int ans;
};

constexpr int64_t IT_COUNT = 700000;

template<typename engine_type>
double time_bench(const std::string& script_path) {
    engine_type engine;
    engine.add_new_script(script_path, script_path);

    auto raw_ptr = engine.alloc_memory_in_wasm_script(script_path, sizeof(test_sum_t));
    assert(raw_ptr != nullptr);
    auto obj_ptr = reinterpret_cast<test_sum_t*>(raw_ptr);

    double min_time = std::numeric_limits<double>::max();

    for (int i = 0; i < IT_COUNT; ++i) {
        new (raw_ptr) test_sum_t(i, i + 1);

        auto start = std::chrono::high_resolution_clock::now();
        engine.run_script(script_path, raw_ptr);
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> time = end - start;
        min_time = std::min(min_time, time.count());

        obj_ptr->~test_sum_t();

    }
    engine.delete_memory_in_wasm_script(script_path, raw_ptr);
    return min_time;
}

void wasm_inside_v8() {
    std::cout << "V8 wasm bench: " << time_bench<v8_instance_db>("./examples/sum_wasm.js") << std::endl;
}

int main() {
    wasm_inside_v8();
    return 0;
}
