#pragma once

#include <memory>
#include <string>
#include <immintrin.h>
#include <vector>

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
//    std::vector<typename __m128i> indexes;
//    std::vector<typename __m128> thresholds;
//    std::vector<__m128> values;
    std::vector<int> indexes;
    std::vector<double> thresholds;
    std::vector<double> values;
};

std::shared_ptr<OptimizedModel> Optimize(const Model& model);

double ApplyOptimizedModel(const OptimizedModel& model, const std::vector<float>& features);
