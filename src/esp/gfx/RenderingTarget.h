// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <Magnum/Magnum.h>
#include <gl_tensor_param.h>

#include "esp/core/esp.h"

#include "WindowlessContext.h"

namespace esp {
namespace gfx {

class RenderingTarget {
 public:
  RenderingTarget(WindowlessContext::ptr context, int width, int height);

  ~RenderingTarget() { LOG(INFO) << "Deconstructing RenderingTarget"; }

  void renderEnter();
  void renderExit();

  gltensor::GLTensorParam::ptr glTensorParamRgba() const;
  gltensor::GLTensorParam::ptr glTensorParamDepth() const;
  gltensor::GLTensorParam::ptr glTensorParamId() const;

  void readFrameRgba(uint8_t* ptr);
  void readFrameDepth(float* ptr);
  void readFrameObjectId(uint32_t* ptr);

  Magnum::Vector2i framebufferSize() const;

  ESP_SMART_POINTERS_WITH_UNIQUE_PIMPL(RenderingTarget);
};

}  // namespace gfx
}  // namespace esp