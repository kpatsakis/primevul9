LogicalResult CstrBroadcastableOpLowering::matchAndRewrite(
    shape::CstrBroadcastableOp op, mlir::PatternRewriter& rewriter) const {
  ShapeComponentAnalysis shape_component_analysis;
  if (!isKnownBroadcastable(shape_component_analysis, op.getShapes(),
                            op.getShapes().front()))
    return failure();

  // Replace constraint with a true witness.
  rewriter.replaceOpWithNewOp<shape::ConstWitnessOp>(op, true);

  return success();
}