InlineHeaderVector& getInVec(VariantHeader& buffer) {
  return absl::get<InlineHeaderVector>(buffer);
}