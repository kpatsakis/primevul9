const InlineHeaderVector& getInVec(const VariantHeader& buffer) {
  return absl::get<InlineHeaderVector>(buffer);
}