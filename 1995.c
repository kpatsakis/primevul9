bool isKnownBroadcastable(ShapeComponentAnalysis& analysis,
                          ValueRange bcasted_shapes, Value output_shape) {
  auto output_shape_dims = analysis.GetValueInfo(output_shape);
  if (!output_shape_dims) return false;
  for (Value shape : bcasted_shapes) {
    auto shape_dims = analysis.GetValueInfo(shape);
    if (!shape_dims) return false;
    // Iterate backwards over the smallest input shape.
    for (auto zip : llvm::zip(llvm::reverse(*output_shape_dims),
                              llvm::reverse(*shape_dims))) {
      const auto& first = std::get<0>(zip);
      const auto& second = std::get<1>(zip);
      // TODO(ezhulenev): What to do with dimensions statically known to be
      // zero?
      // Numpy can only broadcast [0] with [1], however Tensorflow can broadcast
      // [0] with any dimension size, and produces dimension of size [0].
      // Currently we'll conservatively return failure and will not proceed with
      // a rewrite.
      if (first.isConstant(0) || second.isConstant(0)) return false;
      // If either shape has a static one dimension the broadcast will always
      // succeed.
      if (first.isConstant(1) || second.isConstant(1)) continue;
      // Otherwise dims have to be equal.
      if (first != second) return false;
    }
  }
  return true;
}