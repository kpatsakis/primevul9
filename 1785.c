Status DependencyOptimizer::OptimizeDependencies() {
  SetVector<int> nodes_to_simplify;
  std::set<int> nodes_to_delete;
  for (int i = 0; i < optimized_graph_->node_size(); ++i) {
    const NodeDef& node = optimized_graph_->node(i);
    if (IsNoOp(node) || IsIdentity(node) || IsIdentityN(node) ||
        IsConstant(node) || SafeToConvertToNoOp(node)) {
      nodes_to_simplify.PushBack(i);
    }
  }
  while (!nodes_to_simplify.Empty()) {
    int node_to_simplify = nodes_to_simplify.PopBack();
    // Discard nodes that were marked for deletion already.
    while (nodes_to_delete.find(node_to_simplify) != nodes_to_delete.end()) {
      node_to_simplify = nodes_to_simplify.PopBack();
    }
    OptimizeNode(node_to_simplify, &nodes_to_simplify, &nodes_to_delete);
  }

  if (fetch_nodes_known_) {
    VLOG(1) << "Deleted " << nodes_to_delete.size() << " out of "
            << optimized_graph_->node_size() << " nodes.";
    EraseNodesFromGraph(nodes_to_delete, optimized_graph_);
    node_map_.reset(new NodeMap(optimized_graph_));
    BuildNodeToIdx();
  }
  return Status::OK();
}