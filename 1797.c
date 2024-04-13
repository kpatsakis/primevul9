void DependencyOptimizer::BuildNodeToIdx() {
  // Set up &node -> index map.
  node_to_idx_.clear();
  for (int i = 0; i < optimized_graph_->node_size(); ++i) {
    const NodeDef& node = optimized_graph_->node(i);
    node_to_idx_[&node] = i;
  }
}