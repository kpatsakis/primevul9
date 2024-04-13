int DependencyOptimizer::NumEdgesIfBypassed(
    const NodeDef& node, const std::vector<NodeDef*>& output_nodes) const {
  const bool is_multi_input_identity_n =
      IsIdentityN(node) && !IsIdentityNSingleInput(node);
  const int num_outputs = output_nodes.size();
  const int num_inputs = node.input_size();

  if (is_multi_input_identity_n) {
    // multi-input identity_n with input/output control dependencies will likely
    // increase number of edges after optimization.
    int num_edges_if_bypassed(0);
    for (const string& input_node_name : node.input()) {
      if (IsControlInput(input_node_name)) {
        num_edges_if_bypassed += num_outputs;
      } else {
        ++num_edges_if_bypassed;
      }
    }

    for (auto consumer : output_nodes) {
      for (int j = 0; j < consumer->input_size(); ++j) {
        const TensorId consumer_input = ParseTensorName(consumer->input(j));
        if (consumer_input.node() == node.name()) {
          if (IsControlInput(consumer_input)) {
            num_edges_if_bypassed += num_inputs;
          } else {
            ++num_edges_if_bypassed;
          }
        }
      }
    }
    return num_edges_if_bypassed;
  } else {
    return num_inputs * num_outputs;
  }
}