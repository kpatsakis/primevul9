void DependencyOptimizer::CleanControlInputs() {
  for (int i = 0; i < optimized_graph_->node_size(); ++i) {
    DedupControlInputs(optimized_graph_->mutable_node(i));
  }
}