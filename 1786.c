bool RemoveControlInput(NodeDef* node, const string& control_input_to_remove,
                        NodeMap* node_map) {
  for (int pos = node->input_size() - 1; pos >= 0; --pos) {
    const string& input = node->input(pos);
    if (input[0] != '^') break;
    if (input == control_input_to_remove) {
      node->mutable_input()->SwapElements(pos, node->input_size() - 1);
      node->mutable_input()->RemoveLast();
      node_map->RemoveOutput(NodeName(input), node->name());
      return true;
    }
  }
  return false;
}