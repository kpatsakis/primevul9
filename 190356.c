std::string maybeMove(std::string const& other, bool move) {
  if (move) {
    return "std::move(" + other + ")";
  }
  return other;
}