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

#include "ppl/nn/engines/cuda/optimizer/ops/onnx/equal_op.h"

#include "ppl/nn/common/logger.h"
#include "ppl/nn/engines/cuda/kernels/onnx/equal_kernel.h"
#include "ppl/nn/oputils/onnx/reshape_equal.h"

using namespace std;
using namespace ppl::common;

namespace ppl { namespace nn { namespace cuda {

RetCode EqualOp::Init(const OptKernelOptions& options) {
    infer_type_func_ = [this](InputOutputInfo* info, datatype_t type) -> RetCode {
        auto shape = &info->GetOutput<TensorImpl>(0)->GetShape();
        shape->SetDataType(DATATYPE_BOOL);
        return RC_SUCCESS;
    };

    infer_dims_func_ = [this](InputOutputInfo* info) -> RetCode {
        return oputils::ReshapeEqual(info, nullptr);
    };

    return RC_SUCCESS;
}

RetCode EqualOp::Finalize(const OptKernelOptions& options) {
    auto status = SetCommonParam(options);
    if (status != RC_SUCCESS) {
        LOG(ERROR) << "load common param failed: " << GetRetCodeStr(status);
        return status;
    }

    return RC_SUCCESS;
}

KernelImpl* EqualOp::CreateKernelImpl() const {
    return CreateKernelImplWithoutParam<EqualKernel>();
}

}}} // namespace ppl::nn::cuda