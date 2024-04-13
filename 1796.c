bool DependencyOptimizer::SafeToConvertToNoOp(const NodeDef& node) const {
  if (HasRegularOutputs(node, *node_map_)) {
    // The output values of this node may be needed.
    VLOG(3) << "Not safe to convert '" << node.name()
            << " to NoOp. Node has outputs.";
    return false;
  }
  if (!fetch_nodes_known_) {
    VLOG(3) << "Not safe to convert '" << node.name()
            << " to NoOp. Fetches unknown.";
    return false;
  }
  if (nodes_to_preserve_.find(node.name()) != nodes_to_preserve_.end()) {
    VLOG(3) << "Not safe to convert to NoOp: " << node.name()
            << " is in preserve set.";
    return false;
  }
  if (IsMerge(node) || IsSwitch(node) || ModifiesFrameInfo(node)) {
    VLOG(3) << "Not safe to convert '" << node.name()
            << " to NoOp. Node modifies frame info.";
    return false;
  }
  // Ops reading variables are marked as stateful, but are safe to remove if
  // redundant.
  static const absl::flat_hash_set<string>* gather_ops =
      new absl::flat_hash_set<string>{"Gather", "GatherV2", "GatherNd",
                                      "ResourceGather", "ResourceGatherNd"};
  const bool is_variable_read =
      IsReadVariableOp(node) || IsReadVariablesOp(node) ||
      gather_ops->find(node.op()) != gather_ops->end();
  if (!is_variable_read && !IsFreeOfSideEffect(node)) {
    VLOG(3) << "Not safe to convert '" << node.name()
            << " to NoOp. Node has side effect.";
    return false;
  }
  if (node.op().rfind("Submodel", 0) == 0) {
    return false;
  }
  const OpDef* op_def = nullptr;
  Status status = OpRegistry::Global()->LookUpOpDef(node.op(), &op_def);
  if (!status.ok() || op_def->output_arg_size() == 0) {
    return false;
  }
  const std::unordered_set<string> do_not_rewrite_ops{
      "Assert",     "CheckNumerics",         "_Retval",
      "_Arg",       "_ParallelConcatUpdate", "TPUExecute",
      "TPUCompile", "ControlTrigger"};
  if (do_not_rewrite_ops.find(node.op()) != do_not_rewrite_ops.end()) {
    return false;
  }
  if (!SafeToRemoveIdentity(node)) {
    return false;
  }
  return true;
}