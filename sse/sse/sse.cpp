#include "sse.h"

std::shared_ptr<OptimizedModel> Optimize(const Model& model) {
    auto optimizedModel = std::make_shared<OptimizedModel>();
//    Rule emptyRule{0, 0, 0};

    for (size_t i = 0; i < model.size(); ++i) {
        optimizedModel->values.push_back(model[i].value);
        optimizedModel->indexes.push_back(model[i].index);
        optimizedModel->thresholds.push_back(model[i].threshold);
    }

//    for (size_t i = 0; i < model.size(); i + 4) {
//        auto& rule1 = (i + 4 >= model.size()) ? emptyRule : model[i];
//        auto& rule2 = (i + 4 >= model.size()) ? emptyRule : model[i + 1];
//        auto& rule3 = (i + 4 >= model.size()) ? emptyRule : model[i + 2];
//        auto& rule4 = (i + 4 >= model.size()) ? emptyRule : model[i + 3];
//
//        optimizedModel->indexes.push_back(_mm_set_epi32(rule4.index, rule3.index, rule2.index, rule1.index));
//        optimizedModel->thresholds.push_back(_mm_set_epi32(rule4.threshold, rule3.threshold, rule2.threshold, rule1.threshold));
//        optimizedModel->values.push_back(rule1.value);
//        optimizedModel->values.push_back(rule2.value);
//        optimizedModel->values.push_back(rule3.value);
//        optimizedModel->values.push_back(rule4.value);
////        optimizedModel->values.push_back(_mm_set_ps(rule4.values, rule3.values, rule2.values, rule1.values));
//    }
    return optimizedModel;
}

double ApplyOptimizedModel(const OptimizedModel& model, const std::vector<float>& features) {
    double result = 0.0;

    size_t i = 0;
    for (; i + 3 < model.indexes.size(); i += 4) {
        __m128i indices = _mm_set_epi32(model.indexes[i + 3], model.indexes[i + 2], model.indexes[i + 1], model.indexes[i]);
        __m128 thresholds = _mm_set_ps(model.thresholds[i + 3], model.thresholds[i + 2], model.thresholds[i + 1], model.thresholds[i]);

        __m128 features_values = _mm_i32gather_ps(features.data(), indices, 4);
        __m128 mask = _mm_cmpgt_ps(features_values, thresholds);

        __m128d mask_d = _mm_castps_pd(mask);

        __m128d rule_values1 = _mm_set_pd(model.values[i+1], model.values[i]);
        __m128d rule_values2 = _mm_set_pd(model.values[i+3], model.values[i+2]);

        __m128d masked_values1 = _mm_and_pd(mask_d, rule_values1);
        __m128d masked_values2 = _mm_and_pd(mask_d, rule_values2);

        result += _mm_cvtsd_f64(masked_values1) + _mm_cvtsd_f64(_mm_unpackhi_pd(masked_values1, masked_values1));
        result += _mm_cvtsd_f64(masked_values2) + _mm_cvtsd_f64(_mm_unpackhi_pd(masked_values2, masked_values2));
    }

    for (; i < model.indexes.size(); ++i) {
        if (features[model.indexes[i]] > model.thresholds[i]) {
            result += model.values[i];
        }
    }

    return result;
}