// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include "reshape_linear.h"

#include "ppl/nn/runtime/tensor_impl.h"
#include "ppl/nn/common/logger.h"

using namespace ppl::common;

namespace ppl { namespace nn { namespace opmx {

ppl::common::RetCode ReshapeLinear(
    InputOutputInfo* info, const ir::Attr* arg, 
    int64_t in_features_pack_size, int64_t out_features_pack_size,
    bool check_weight_shape) {
    
    auto param = static_cast<const LinearParam*>(arg);
    const TensorShape& input_shape = *info->GetInput<TensorImpl>(0)->GetShape();
    const TensorShape& weight_shape = *info->GetInput<TensorImpl>(1)->GetShape();
    const uint32_t out_dim_count = input_shape.GetDimCount();

    if(input_shape.GetDim(out_dim_count - 1) != param->in_features) {
        LOG(ERROR) << info->GetNode()->GetName() <<
            " in_features(" << param->in_features <<
            ") not equal to input's last dim(" <<
            input_shape.GetDim(out_dim_count - 1) << ")";
        return RC_INVALID_VALUE;
    }

    if (check_weight_shape && weight_shape.GetDim(0) * out_features_pack_size != param->out_features) {
        LOG(ERROR) << info->GetNode()->GetName() <<
                " out_features(" << param->out_features <<
                ") not equal to weight's out feature dim(" <<
                weight_shape.GetDim(0) * out_features_pack_size << ")";
        return RC_INVALID_VALUE;
    }

    std::vector<int64_t> out_dims(out_dim_count);
    for (uint32_t i = 0; i < out_dim_count; ++i) {
        out_dims[i] = input_shape.GetDim(i);
    }
    out_dims[out_dim_count - 1] = param->out_features;
    info->GetOutput<TensorImpl>(0)->GetShape()->Reshape(out_dims);
    
    return RC_SUCCESS;
}

}}} // namespace ppl::nn::opmx


