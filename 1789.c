void LongestPathsLowerBounds(
    int source, const std::pair<int, int>& target_range,
    const std::vector<std::vector<int>>& outputs,
    std::vector<DistanceFromSource>* longest_distance) {
  std::deque<int> queue;
  queue.emplace_front(source);
  while (!queue.empty()) {
    int node = queue.front();
    queue.pop_front();
    for (int fanout : outputs[node]) {
      // 1) Only nodes in the target range can be on paths from source to one of
      //    its control outputs.
      // 2) Since we only need a lower bound on the longest distance, we can
      //    skip nodes for which we have already proven have a path of
      //    length > 1 from the source.
      if (fanout >= target_range.first && fanout <= target_range.second &&
          (*longest_distance)[fanout] != TWO_OR_GREATER) {
        (*longest_distance)[fanout] =
            (*longest_distance)[fanout] == ZERO ? ONE : TWO_OR_GREATER;
        queue.emplace_front(fanout);
      }
    }
  }
}