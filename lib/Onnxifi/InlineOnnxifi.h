/**
 * Copyright (c) 2017-present, Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef GLOW_ONNXIFI_INLINEONNXIFI_H
#define GLOW_ONNXIFI_INLINEONNXIFI_H

#include "Base.h"

#include "llvm/ADT/SmallVector.h"

namespace glow {
namespace onnxifi {

enum class OnnxifiQuantizationStep : char {
  None,     // Don't perform any quantization steps
  Profile,  // Insert profiling nodes and dump quantization profiles
  Quantize, // Load quantization profile and insert quantize/dequantize nodes
};

/// Onnxifi Graph whose run method just executes the underlying function on the
/// same thread that calls its setIOAndRun function.
class InlineGraph : public Graph {
public:
  InlineGraph(BackendPtr backendPtr, OnnxifiQuantizationStep quantizationStep)
      : Graph(backendPtr), quantizationStep_(quantizationStep) {}

  /// Init Glow graph based on the ONNX model \p onnxModel and
  /// static trained weights \p weightDescriptors.
  onnxStatus
  initGraph(const void *onnxModel, size_t onnxModelSize, uint32_t weightCount,
            const onnxTensorDescriptorV1 *weightDescriptors) override;

  onnxStatus run(std::unique_ptr<ExecutionContext> ctx, EventPtr outputEvent,
                 std::unordered_map<Placeholder *, onnxTensorDescriptorV1>
                     phNameToOnnxTensorOutputs,
                 onnxTraceEventList *traceEvents) override;

private:
  ExecutionEngine executionEngine_;
  Function *function_;
  OnnxifiQuantizationStep quantizationStep_;

  /// A map between quantization profiling names of NodeValues that were lowered
  /// from each other. Maps to a set of NodeValues that were replaced by the
  /// NodeValue (key) that replaced them.
  LoweredInfoMap loweredMap_;

  /// Hash of the model, used to find profiling data.
  llvm::SmallString<32> modelHash_;
};

} // namespace onnxifi
} // namespace glow

#endif // GLOW_ONNXIFI_INLINEONNXIFI_H