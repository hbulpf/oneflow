#ifndef ONEFLOW_CORE_COMPILER_OF2XLA_XLA_LAUNCH_KERNEL_H_
#define ONEFLOW_CORE_COMPILER_OF2XLA_XLA_LAUNCH_KERNEL_H_

#include "oneflow/core/kernel/kernel.h"

namespace oneflow {

template <DeviceType device_type, typename T>
class XlaLaunchKernel : public KernelIf<device_type> {
 public:
  XlaLaunchKernel() = default;
  virtual ~XlaLaunchKernel() {}

 private:
  void ForwardDataContent(const KernelCtx&,
                          std::function<Blob*(const std::string&)>) const override;
};

}  // namespace oneflow

#endif  // ONEFLOW_CORE_COMPILER_OF2XLA_XLA_LAUNCH_KERNEL_H_

