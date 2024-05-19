#include "sse.h"
#include <immintrin.h>

struct OptimizedModel {
    //    std::vector<__m128i> indexes_simd;
//    std::vector<__m128> thresholds_simd;
//    std::vector<__m128> values_simd;

//    size_t model_size = 0;

    std::vector<int> indexes;
    std::vector<float> thresholds;
    std::vector<float> values;
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
    auto indexes_data = (__m128i*) model.indexes.data();
    auto thresholds_data = (__m128*) model.thresholds.data();
    auto values_data = (__m128*) model.values.data();

    __m128 result = _mm_setzero_ps();
    for (size_t i = 0; i < model.indexes.size() / 4; ++i, ++indexes_data, ++thresholds_data, ++values_data) {
//        __m128i indices = _mm_set_epi32(model.indexes[i + 3], model.indexes[i + 2], model.indexes[i + 1],
//                                        model.indexes[i]);
//        __m128 thresholds = _mm_set_ps(model.thresholds[i + 3], model.thresholds[i + 2], model.thresholds[i + 1],
//                                       model.thresholds[i]);

        __m128 features_vec = _mm_setr_ps(features[((int*) indexes_data)[3]],
                                          features[((int*) indexes_data)[2]],
                                          features[((int*) indexes_data)[1]],
                                          features[((int*) indexes_data)[0]]);
        __m128 mask = _mm_cmpgt_ps(features_vec, *thresholds_data);

//        __m128 rule_values = _mm_set_ps(model.values[i + 3], model.values[i + 2], model.values[i + 1], model.values[i]);

        __m128 masked_values = _mm_and_ps(mask, *values_data);

        result = _mm_add_ps(result, masked_values);
    }

    double d_result = result[0] + result[1] + result[2] + result[3];
    for (size_t i = model.indexes.size() - model.indexes.size() % 4; i < model.indexes.size(); ++i) {
        if (features[model.indexes[i]] > model.thresholds[i]) {
            d_result += model.values[i];
        }
    }

    return d_result;
}
