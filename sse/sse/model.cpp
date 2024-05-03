#include "sse.h"

#include <fstream>

std::vector<std::vector<float>> LoadDataset(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("can't open file '" + path + "'");
    }

    int n, m;
    in >> n >> m;

    std::vector<std::vector<float>> dataset;
    for (int i = 0; i < n; ++i) {
        auto& element = dataset.emplace_back(m);
        for (int j = 0; j < m; ++j) {
            in >> element[j];
        }
    }

    return dataset;
}

Model LoadModel(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("can't open file '" + path + "'");
    }

    int n;
    in >> n;
    Model model;
    for (int i = 0; i < n; ++i) {
        auto& rule = model.emplace_back();
        in >> rule.index >> rule.threshold >> rule.value;
    }

    return model;
}

double ApplyModel(const Model& model, const std::vector<float>& features) {
    double result = 0;
    for (const auto& rule : model) {
        if (features[rule.index] > rule.threshold) {
            result += rule.value;
        }
    }
    return result;
}
