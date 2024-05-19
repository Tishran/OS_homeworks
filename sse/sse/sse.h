#pragma once

#include <memory>
#include <string>
#include <immintrin.h>
#include <vector>
#include <variant>

std::vector<std::vector<float>> LoadDataset(const std::string& path);

struct Rule {
    int index;
    double threshold;
    double value;
};

using Model = std::vector<Rule>;

Model LoadModel(const std::string& path);

double ApplyModel(const Model& model, const std::vector<float>& features);

struct OptimizedModel {
//    std::vector<__m128i> indexes_simd;
//    std::vector<__m128> thresholds_simd;
//    std::vector<__m128> values_simd;

//    size_t model_size = 0;

    std::vector<int> indexes;
    std::vector<double> thresholds;
    std::vector<double> values;
};

std::shared_ptr<OptimizedModel> Optimize(const Model& model);

double ApplyOptimizedModel(const OptimizedModel& model, const std::vector<float>& features);
