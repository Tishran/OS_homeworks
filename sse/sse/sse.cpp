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

        __m128 features_vec = _mm_setr_ps(features[((int*) indexes_data)[0]],
                                          features[((int*) indexes_data)[1]],
                                          features[((int*) indexes_data)[2]],
                                          features[((int*) indexes_data)[3]]);
        __m128 mask = _mm_cmpgt_ps(features_vec, *thresholds_data);

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
