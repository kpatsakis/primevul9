llvm::Optional<AffineMap> isNonExpandingBroadcast(
    ShapeComponentAnalysis& analysis, Value from, Value to_shape) {
  auto in_shape = analysis.GetShapeInfo(from);
  auto out_shape = analysis.GetValueInfo(to_shape);
  if (!in_shape || !out_shape) return {};

  SmallVector<AffineExpr> input_map_exprs;
  size_t rank = out_shape->size();
  MLIRContext* ctx = (*out_shape)[0].expr.getContext();
  size_t d = 0;
  auto affine_zero = getAffineConstantExpr(0, ctx);
  for (auto zip :
       llvm::zip(llvm::reverse(*in_shape), llvm::reverse(*out_shape))) {
    const auto& in = std::get<0>(zip);
    const auto& out = std::get<1>(zip);
    bool extend = in.isConstant(1) && !out.isConstant(1);
    input_map_exprs.push_back(extend ? affine_zero
                                     : getAffineDimExpr(rank - d - 1, ctx));
    ++d;

    // Bail if this is neither a known expansion nor a known non-expansion.
    if (!extend && in != out) return {};
  }
  // Any leading dimensions will be expanded.
  input_map_exprs.resize(in_shape->size(), affine_zero);
  std::reverse(input_map_exprs.begin(), input_map_exprs.end());
  return AffineMap::get(/*dimCount=*/rank,
                        /*symbolCount=*/0, input_map_exprs, ctx);
}