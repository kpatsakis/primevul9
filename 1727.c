int GetOutputSize(int max_seen, int max_length, int min_length) {
  return max_length > 0 ? max_length : std::max((max_seen + 1), min_length);
}