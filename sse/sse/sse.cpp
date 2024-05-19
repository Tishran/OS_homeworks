#include "sse.h"
#include <immintrin.h>

struct OptimizedModel {
    //    std::vector<__m128i> indexes_simd;
//    std::vector<__m128> thresholds_simd;
//    std::vector<__m128> values_simd;

//    size_t model_size = 0;

    std::vector<int> indexes;
    std::vector<double> thresholds;
    std::vector<double> values;
};

std::shared_ptr<OptimizedModel> Optimize(const Model &model) {
    std::shared_ptr<OptimizedModel> optimizedModel = std::make_shared<OptimizedModel>();
//    Rule emptyRule{0, 0, 0};

    for (const auto &i: model) {
//        auto& rule1 = model[i];
//        auto& rule2 = (i + 1 < model.size()) ? model[i + 1] : emptyRule;
//        auto& rule3 = (i + 1 < model.size()) ? model[i + 2] : emptyRule;
//        auto& rule4 = (i + 1 < model.size()) ? model[i + 3] : emptyRule;

//        optimizedModel->indexes_simd.push_back(_mm_set_epi32(rule4.index, rule3.index, rule2.index, rule1.index));
//        optimizedModel->thresholds_simd.push_back(_mm_set_ps(rule4.threshold, rule3.threshold, rule2.threshold, rule1.threshold));
//        optimizedModel->values_simd.push_back(_mm_set_ps(rule4.value, rule3.value, rule2.value, rule1.value));

        optimizedModel->indexes.push_back(i.index);
        optimizedModel->values.push_back(i.value);
        optimizedModel->thresholds.push_back(i.threshold);
    }

    return optimizedModel;
}

double ApplyOptimizedModel(const OptimizedModel &model, const std::vector<float> &features) {
    double result = 0.0;

    size_t i = 0;
    for (; i + 3 < model.indexes.size(); i += 4) {
        __m128i indices = _mm_set_epi32(model.indexes[i + 3], model.indexes[i + 2], model.indexes[i + 1],
                                        model.indexes[i]);
        __m128 thresholds = _mm_set_ps(model.thresholds[i + 3], model.thresholds[i + 2], model.thresholds[i + 1],
                                       model.thresholds[i]);

        __m128 features_values = _mm_set_ps(features[_mm_extract_epi32(indices, 3)], features[_mm_extract_epi32(indices, 2)],
                                            features[_mm_extract_epi32(indices, 1)], features[_mm_extract_epi32(indices, 0)]);
        __m128 mask = _mm_cmpgt_ps(features_values, thresholds);

        __m128 rule_values = _mm_set_ps(model.values[i + 3], model.values[i + 2], model.values[i + 1], model.values[i]);

        __m128 masked_values = _mm_and_ps(mask, rule_values);

        result += masked_values[0];
        result += masked_values[1];
        result += masked_values[2];
        result += masked_values[3];
    }

    for (; i < model.indexes.size(); ++i) {
        if (features[model.indexes[i]] > model.thresholds[i]) {
            result += model.values[i];
        }
    }

    return result;
}
