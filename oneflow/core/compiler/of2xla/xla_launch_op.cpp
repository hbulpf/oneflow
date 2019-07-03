#include "absl/strings/str_split.h"
#include "oneflow/core/compiler/of2xla/xla_utility.h"
#include "oneflow/core/compiler/of2xla/xla_node.h"
#include "oneflow/core/compiler/of2xla/xla_graph.h"
#include "oneflow/core/compiler/of2xla/xla_launch_op.h"
#include "oneflow/core/job/sbp_signature_builder.h"

namespace oneflow {

void XlaLaunchOp::InitFromOpConf() {
  CHECK(op_conf().has_xla_launch_conf());

  EnrollRepeatedInputBn("in");
  EnrollRepeatedOutputBn("out");
  // Setup subgraph
  subgraph_.reset(new mola::XlaLaunchGraph(op_conf().xla_launch_conf()));
}

const PbMessage &XlaLaunchOp::GetCustomizedConf() const {
  return op_conf().xla_launch_conf();
}

void XlaLaunchOp::InferBlobDescs(
    std::function<BlobDesc*(const std::string&)> GetBlobDesc4BnInOp,
    const ParallelContext* parallel_ctx) const {
  // Prepare outer input blob descs
  std::unordered_map<std::string, BlobDesc> blob_descs;
  for (const std::string &bn : this->input_bns()) {
    const LogicalBlobId& lbi = this->BnInOp2Lbi(bn);
    std::string blob_name = BlobName(lbi);
    blob_descs[blob_name] = *GetBlobDesc4BnInOp(bn);
  }
  // Inference blob descs in subgraph
  subgraph_->InferBlobDescs(&blob_descs, parallel_ctx);

  // Fetch output blob descs
  for (const std::string &bn : this->output_bns()) {
    CHECK_GT(blob_descs.count(bn), 0);
    *GetBlobDesc4BnInOp(bn) = blob_descs[bn];
  }
}

void XlaLaunchOp::InferHasBatchDim(
    std::function<bool*(const std::string&)> HasBatchDim4BnInOp) const {
  const auto &xla_launch_conf = op_conf().xla_launch_conf();
  const auto &batch_dim_blobs = xla_launch_conf.attr().batch_dim_blob();
  auto has_batch_dim_fn = [&](const std::string &blob_name) {
    return (std::find(batch_dim_blobs.begin(), batch_dim_blobs.end(), blob_name)
                != batch_dim_blobs.end());
  };
  for (const std::string &bn : this->output_bns()) {
    LogicalBlobId lbi = subgraph_->Output(bn);
    std::string blob_name = BlobName(lbi);
    *HasBatchDim4BnInOp(bn) = has_batch_dim_fn(blob_name);
  }
}

void XlaLaunchOp::InferSbpSignature(
    SbpSignature* sbp_signature, const SbpSignature& sbp_sig_conf,
    const std::function<int32_t(const SbpSignature&)>& CalcOrderValue4SbpSig,
    std::function<const SbpInferHint&(const std::string&)> SbpInferHint4Ibn,
    const ParallelDesc& parallel_desc) const {
  UNIMPLEMENTED();
  // const auto &xla_launch_conf = op_conf().xla_launch_conf();
  // const auto &attr_sbp_conf = xla_launch_conf.attr().sbp_signature();
  // SbpSignature sbp_conf;
  // auto *bn2sbp_parallel = sbp_conf.mutable_bn_in_op2sbp_parallel();
  // for (const std::string &bn : this->input_bns()) {
  //   std::string blob_name = BlobName(this->BnInOp2Lbi(bn));
  //   const auto &it = subgraph_inputs_.find(blob_name);
  //   CHECK(it != subgraph_inputs_.end());
  //   CHECK_GT(attr_sbp_conf.count(it->second), 0);
  //   (*bn2sbp_parallel)[bn] = attr_sbp_conf.at(it->second);
  // }
  // for (const std::string &bn : this->input_bns()) {
  //   const auto &it = subgraph_outputs_.find(bn);
  //   CHECK(it != subgraph_outputs_.end());
  //   CHECK_GT(attr_sbp_conf.count(it->second), 0);
  //   (*bn2sbp_parallel)[bn] = attr_sbp_conf.at(it->second);
  // }
  // this->InferSbpSignature(sbp_signature, sbp_conf, CalcOrderValue4SbpSig,
  //                         SbpInferHint4Ibn, parallel_desc);
}

REGISTER_OP(OperatorConf::kXlaLaunchConf, XlaLaunchOp);

}  // namespace oneflow
