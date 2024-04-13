Status DependencyOptimizer::Optimize(Cluster* cluster, const GrapplerItem& item,
                                     GraphDef* optimized_graph) {
  optimized_graph_ = optimized_graph;
  *optimized_graph_ = item.graph;
  nodes_to_preserve_ = item.NodesToPreserve();
  fetch_nodes_known_ = !item.fetch.empty();
  CleanControlInputs();

  const int num_iterations = 2;
  for (int iteration = 0; iteration < num_iterations; ++iteration) {
    GRAPPLER_RETURN_IF_DEADLINE_EXCEEDED();
    Status topo_sort_status;
    // Perform topological sort to prepare the graph for transitive reduction.
    topo_sort_status = TopologicalSort(optimized_graph_);
    // Set up index-based graph datastructures to speed up analysis steps below.
    node_map_.reset(new NodeMap(optimized_graph_));
    BuildNodeToIdx();

    if (topo_sort_status.ok()) {
      // Remove redundant control dependencies.
      TF_RETURN_IF_ERROR(TransitiveReduction());
    } else {
      LOG(ERROR) << "Iteration = " << iteration
                 << ", topological sort failed with message: "
                 << topo_sort_status.error_message();
    }
    // Turn nodes with only control outputs into NoOps, prune NoOp and Identity
    // nodes.
    TF_RETURN_IF_ERROR(OptimizeDependencies());

    // Dedup control inputs.
    CleanControlInputs();

    // Merge multiple control edges from the same device.
    GroupCrossDeviceControlEdges(/*host_granularity=*/false);

    // Merge control edges from the same host to reduce RPC traffic.
    GroupCrossDeviceControlEdges(/*host_granularity=*/true);
  }

  return Status::OK();
}