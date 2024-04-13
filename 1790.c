void DependencyOptimizer::OptimizeNode(int node_idx,
                                       SetVector<int>* nodes_to_simplify,
                                       std::set<int>* nodes_to_delete) {
  NodeDef* node = optimized_graph_->mutable_node(node_idx);
  const bool is_noop = IsNoOp(*node);
  const bool is_identity = IsIdentity(*node) || IsIdentityNSingleInput(*node);
  const bool is_multi_input_identity =
      IsIdentityN(*node) && !IsIdentityNSingleInput(*node);
  const string node_name = node->name();
  // Constant nodes with no input control dependency are always executed early,
  // so we can prune all their output control dependencies.
  if (IsConstant(*node) && node->input_size() == 0) {
    const auto output_nodes = node_map_->GetOutputs(node_name);
    for (NodeDef* fanout : output_nodes) {
      bool optimize_fanout = false;
      bool data_connection = false;
      for (int i = fanout->input_size() - 1; i >= 0; --i) {
        const TensorId input_tensor = ParseTensorName(fanout->input(i));
        if (input_tensor.node() == node_name) {
          if (input_tensor.index() < 0) {
            fanout->mutable_input()->SwapElements(i, fanout->input_size() - 1);
            fanout->mutable_input()->RemoveLast();
            optimize_fanout = true;
          } else {
            data_connection = true;
          }
        }
      }
      if (optimize_fanout) {
        nodes_to_simplify->PushBack(node_to_idx_[fanout]);
        if (!data_connection) {
          node_map_->RemoveOutput(node_name, fanout->name());
        }
      }
    }
    if (node_map_->GetOutputs(node_name).empty() && fetch_nodes_known_ &&
        nodes_to_preserve_.find(node_name) == nodes_to_preserve_.end()) {
      // Mark the node for deletion.
      nodes_to_delete->insert(node_to_idx_[node]);
    }
    return;
  }

  // Change ops that only have control dependencies as outputs to NoOps.
  if (!is_noop && SafeToConvertToNoOp(*node)) {
    VLOG(2) << "***** Replacing  " << node_name << " (" << node->op()
            << ") with NoOp.";
    // The outputs of this node are not consumed. Replace its inputs with
    // control dependencies and replace the op itself with the NoOp op.
    std::unordered_set<string> ctrl_inputs;
    int pos = 0;
    while (pos < node->input_size()) {
      const string old_input = node->input(pos);
      if (IsControlInput(old_input)) {
        if (!ctrl_inputs.insert(old_input).second) {
          // We found a duplicate control input. Remove it.
          node->mutable_input()->SwapElements(pos, node->input_size() - 1);
          node->mutable_input()->RemoveLast();
        } else {
          ++pos;
        }
        continue;
      }
      // Replace a normal input with a control input.
      const string ctrl_input = ConstantFolding::AddControlDependency(
          old_input, optimized_graph_, node_map_.get());
      ctrl_inputs.insert(ctrl_input);
      node->set_input(pos, ctrl_input);
      node_map_->UpdateInput(node_name, old_input, ctrl_input);
      const NodeDef* old_input_node = node_map_->GetNode(old_input);
      nodes_to_simplify->PushBack(node_to_idx_[old_input_node]);
      ++pos;
    }
    node->set_op("NoOp");
    EraseRegularNodeAttributes(node);
    DedupControlInputs(node);
    nodes_to_simplify->PushBack(node_to_idx_[node]);
    return;
  }

  // Remove NoOp nodes if the product of their fan-in and fan-out is less than
  // or equal to the sum of the fan-in and fan-out. The non-trivial rewrites
  // take the following form:
  //
  // Case a)
  //    x --^> +------+                x --^> +---+
  //    y --^> | NoOp | --^> a   ==>   y --^> | a |
  //    ...    |      |                  ...  |   |
  //    z --^> +------+                z --^> +---+
  //
  // Case b)
  //           +------+ --^> a         +---+ --^> a
  //    x --^> | NoOp | --^> b  ==>    | x | --^> b
  //           |      | ...            |   | ...
  //           +------+ --^> c         +---+ --^> c
  // Case c)
  //           +------+                x ---^> a
  //    x --^> | NoOp | --^> a  ==>      \/
  //    y --^> |      | --^> b           /\
  //           +------+                y ---^> b
  //
  // We only apply this optimization if we don't increase the number of control
  // edges across device boundaries, e.g. in cases a) and b) if NoOp and
  // a and x, respectively, are on the same device. Control edges across device
  // boundaries require inter-device communication (Send/Recv pairs to be
  // inserted in the graph), which is very costly.
  //
  // We also remove identity nodes, subject to the same constraints on number of
  // resulting control edges and device boundary crossings:
  //
  // Case a)
  //          +----------+ ---> a       +---+ ---> a
  //    x --> | Identity | --^> b  ==>  | x | --^> b
  //          |          | ...          |   | ...
  //          +----------+ --^> c       +---+ --^> c
  //
  // Case b)
  //    x ---> +----------+ ---> a      x ---> +---+
  //    y --^> | Identity |        ==>  y --^> | a |
  //    ...    |          |               ...  |   |
  //    z --^> +----------+             z --^> +---+
  //
  // Case c)
  //           +----------+             x ---> +---+
  //    x ---> | Identity | ---> a ==>   \--^> | a |
  //    y --^> |          | --^> b       /\    +---+
  //           +----------+             y --^> b

  if (is_noop || ((is_identity || is_multi_input_identity) &&
                  SafeToRemoveIdentity(*node))) {
    const int num_inputs = node->input_size();
    std::vector<NodeDef*> input_nodes;
    for (int i = 0; i < num_inputs; ++i) {
      NodeDef* input_node = node_map_->GetNode(node->input(i));
      if (input_node == nullptr) {
        LOG(ERROR) << "Invalid input " << node->input(i);
        return;
      }
      input_nodes.push_back(input_node);
    }
    const auto& output_node_set = node_map_->GetOutputs(node_name);
    const std::vector<NodeDef*> output_nodes(output_node_set.begin(),
                                             output_node_set.end());

    if (!BypassingNodeIsBeneficial(*node, input_nodes, output_nodes)) {
      return;
    }

    VLOG(2) << "***** Rerouting input around\n" << node->DebugString();
    // Now remove the node and re-wire its inputs to its outputs.
    for (auto consumer : output_nodes) {
      bool updated_consumer = false;
      VLOG(2) << "consumer before:\n" << consumer->DebugString();
      // Remove dependency on node from consumer.
      for (int i = 0; i < num_inputs; ++i) {
        const NodeDef* input = input_nodes[i];
        // Forward dependency from input to consumer if it doesn't already
        // depend on it.
        if ((is_identity && i == 0) ||
            (is_multi_input_identity && !IsControlInput(node->input(i)))) {
          // Replace regular input from Identity node.
          string new_input;
          const string& input_to_forward = node->input(i);
          CHECK(!IsControlInput(input_to_forward));
          for (int j = 0; j < consumer->input_size(); ++j) {
            const TensorId old_input = ParseTensorName(consumer->input(j));
            if (old_input.node() == node_name) {
              if (old_input.index() == i) {
                // Regular input
                new_input = input_to_forward;
                node_map_->UpdateInput(consumer->name(),
                                       string(old_input.node()), new_input);
                consumer->set_input(j, new_input);
              } else if (old_input.index() == -1) {
                // Control dependency
                new_input = AsControlDependency(NodeName(input_to_forward));
                node_map_->UpdateInput(consumer->name(),
                                       string(old_input.node()), new_input);
                consumer->set_input(j, new_input);
              }
            }
          }
          updated_consumer = true;
        } else {
          // Forward dependency from input to consumer if it doesn't already
          // depend on it.
          if (node_map_->GetOutputs(input->name()).count(consumer) == 0) {
            consumer->add_input(AsControlDependency(input->name()));
            node_map_->AddOutput(input->name(), consumer->name());
            nodes_to_simplify->PushBack(node_to_idx_[input]);
            updated_consumer = true;
          }
        }
      }
      updated_consumer |= RemoveControlInput(
          consumer, AsControlDependency(node_name), node_map_.get());
      if (updated_consumer) {
        nodes_to_simplify->PushBack(node_to_idx_[consumer]);
      }
      VLOG(2) << "consumer after:\n" << consumer->DebugString();
    }
    node_map_->RemoveOutputs(node_name);
    if (fetch_nodes_known_ &&
        nodes_to_preserve_.find(node_name) == nodes_to_preserve_.end()) {
      // Mark the node for deletion.
      nodes_to_delete->insert(node_idx);

      // Disconnect the node from its inputs to enable further optimizations.
      node_map_->RemoveInputs(node_name);
      node->clear_input();
    }
  }
}