Status DependencyOptimizer::TransitiveReduction() {
  // PRECONDITION: optimized_graph_ must be sorted topologically.
  const int num_nodes = optimized_graph_->node_size();
  // Set up a compressed version of the graph to save a constant factor in the
  // expensive algorithm below. Also cache the set of control outputs and the
  // highest index of a target of any control output from each node.
  int num_controls = 0;
  std::vector<std::vector<int>> outputs(num_nodes);
  std::vector<gtl::InlinedVector<std::pair<int, int>, 2>> control_outputs(
      num_nodes);
  // target_range[i] contains the range of node indices for which to compute
  // longest paths starting from node i.
  std::vector<std::pair<int, int>> target_range(num_nodes, {num_nodes, -1});
  for (int node_idx = 0; node_idx < num_nodes; ++node_idx) {
    const NodeDef& node = optimized_graph_->node(node_idx);
    if (ModifiesFrameInfo(node) || !HasOpDef(node)) {
      // Ignore function nodes and nodes that modify frame info.
      continue;
    }
    for (int input_slot = 0; input_slot < node.input_size(); ++input_slot) {
      const string& input = node.input(input_slot);
      const NodeDef* input_node = node_map_->GetNode(input);
      if (ModifiesFrameInfo(*input_node) || IsMerge(*input_node)) {
        // Ignore edges from nodes that modify frame info and from Merge nodes,
        // because we cannot know which of it's input paths executes.
        continue;
      }
      const int input_node_idx = node_to_idx_[input_node];
      outputs[input_node_idx].push_back(node_idx);
      target_range[input_node_idx].first =
          std::min(target_range[input_node_idx].first, node_idx);
      if (IsControlInput(input)) {
        ++num_controls;
        control_outputs[input_node_idx].emplace_back(node_idx, input_slot);
        target_range[input_node_idx].second =
            std::max(target_range[input_node_idx].second, node_idx);
      }
    }
  }

  // Run the longest path in DAG algorithm for each source node that has control
  // outputs. If, for any target node of a control output, there exists a path
  // of length > 1, we can drop that control dependency.
  int num_controls_removed = 0;
  std::vector<DistanceFromSource> longest_distance(num_nodes);
  // Map from target_index -> set of (input_slot, source_index), representing
  // the control edges to remove. We sort them in reverse order by input slot,
  // such that when we swap them out so we don't clobber the
  // node(target).input() repeated field.
  typedef std::pair<int, int> InputSlotAndSource;
  absl::flat_hash_map<
      int, std::set<InputSlotAndSource, std::greater<InputSlotAndSource>>>
      control_edges_to_remove;
  for (int source = 0; source < num_nodes; ++source) {
    if (target_range[source].first >= target_range[source].second ||
        target_range[source].second <= source) {
      continue;
    }
    // Compute the set of nodes in the transitive fanout of source with
    // topological sort index in [target_range.first : target_range.second]]
    // to which there exists a path of length 2 or more from source.
    std::fill(longest_distance.begin() + target_range[source].first,
              longest_distance.begin() + target_range[source].second + 1, ZERO);
    LongestPathsLowerBounds(source, target_range[source], outputs,
                            &longest_distance);

    // If the longest path from source to target of a control dependency is
    // longer than 1, there exists an alternate path, and we can eliminate the
    // redundant direct control dependency.
    for (const auto& control_output : control_outputs[source]) {
      const int target = control_output.first;
      if (longest_distance[target] == TWO_OR_GREATER) {
        const int input_slot = control_output.second;
        control_edges_to_remove[target].emplace(input_slot, source);
      }
    }
  }
  for (const auto& it : control_edges_to_remove) {
    const int target = it.first;
    NodeDef* target_node = optimized_graph_->mutable_node(target);
    for (const InputSlotAndSource& slot_and_source : it.second) {
      const int input_slot = slot_and_source.first;
      const int source = slot_and_source.second;
      const NodeDef& source_node = optimized_graph_->node(source);
      CHECK_LT(input_slot, target_node->input_size());
      target_node->mutable_input()->SwapElements(input_slot,
                                                 target_node->input_size() - 1);
      node_map_->RemoveOutput(source_node.name(), target_node->name());
      target_node->mutable_input()->RemoveLast();
      ++num_controls_removed;
    }
  }
  VLOG(1) << "Removed " << num_controls_removed << " out of " << num_controls
          << " control dependencies";
  return Status::OK();
}