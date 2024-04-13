bool DependencyOptimizer::BypassingNodeIsBeneficial(
    const NodeDef& node, const std::vector<NodeDef*>& input_nodes,
    const std::vector<NodeDef*>& output_nodes) const {
  const bool is_identity = IsIdentity(node) || IsIdentityNSingleInput(node);
  const bool is_multi_input_identity_n =
      IsIdentityN(node) && !IsIdentityNSingleInput(node);
  const int num_outputs = output_nodes.size();
  const int num_inputs = node.input_size();

  if (NumEdgesIfBypassed(node, output_nodes) > num_inputs + num_outputs) {
    return false;
  }

  // Make sure that we don't increase the number of edges that cross
  // device boundaries.
  if ((num_inputs == 1 && num_outputs > 1 &&
       input_nodes[0]->device() != node.device()) ||
      (num_inputs > 1 && num_outputs == 1 &&
       output_nodes[0]->device() != node.device())) {
    return false;
  }

  // TODO(rmlarsen): Not all device crossings are equally expensive.
  // Assign a cost to each based on device affinity and compute a
  // cost before and after.
  const string& node_dev = node.device();
  int num_cross_in = 0;
  for (NodeDef* input_node : input_nodes) {
    num_cross_in += static_cast<int>(input_node->device() != node_dev);
  }
  int num_cross_out = 0;
  for (NodeDef* output_node : output_nodes) {
    num_cross_out += static_cast<int>(output_node->device() != node_dev);
  }

  // Make sure we do not increase the number of device crossings.
  const int num_cross_before = num_cross_in + num_cross_out;
  int num_cross_after = 0;
  for (NodeDef* input_node : input_nodes) {
    for (NodeDef* output_node : output_nodes) {
      num_cross_after +=
          static_cast<int>(input_node->device() != output_node->device());
    }
  }
  if (num_cross_after > num_cross_before) {
    return false;
  }

  if ((is_identity || is_multi_input_identity_n) && num_cross_in > 0 &&
      num_cross_out > 0 && num_cross_after > 0) {
    // This identity node follows a device crossing, so it might be
    // following a _Recv node after partitioning. Do not remove such nodes,
    // unless they only have consumers on the same device as themselves.
    return false;
  }

  return true;
}