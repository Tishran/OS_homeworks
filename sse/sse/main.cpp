#include "sse.h"

#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>

constexpr int ITER_COUNT = 30000000;
constexpr int INDEX_MASK = 0xFFF;

void DoNotOptimize(double value) {
    [[maybe_unused]] volatile double v = value;
    asm volatile ("": : :"memory");
}

void BenchmarkModel(const Model& model, const std::vector<std::vector<float>>& dataset) {
    for (int i = 0; i < ITER_COUNT; ++i) {
        DoNotOptimize(ApplyModel(model, dataset[i % INDEX_MASK]));
    }
}

void BenchmarkOptimizedModel(
    const OptimizedModel& model,
    const std::vector<std::vector<float>>& dataset,
    const std::vector<double>& answers
) {
    for (int i = 0; i < ITER_COUNT; ++i) {
        auto result = ApplyOptimizedModel(model, dataset[i % INDEX_MASK]);
        DoNotOptimize(result);

        if (i < INDEX_MASK && fabs(result - answers[i]) > 0.0001) {
            std::cerr << result << " != " << answers[i % INDEX_MASK] << std::endl;
            abort();
        }
    }
}

int main(int argc, const char* argv[]) {
    const bool optimized = argc == 2 && strcmp(argv[1], "--sse") == 0;

    const auto model = LoadModel("model.txt");
    auto dataset = LoadDataset("dataset.txt");
    for (int i = 0; dataset.size() < INDEX_MASK; ++i) {
        dataset.push_back(dataset[i]);
    }

    std::vector<double> answers;
    answers.reserve(dataset.size());
    for (const auto& features : dataset) {
        answers.push_back(ApplyModel(model, features));
    }

    if (optimized) {
        const auto optimized_model = Optimize(model);
        BenchmarkOptimizedModel(*optimized_model, dataset, answers);
    } else {
        BenchmarkModel(model, dataset);
    }

    return 0;
}
