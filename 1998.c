LogicalResult BroadcastOpLowering::matchAndRewrite(
    shape::BroadcastOp op, mlir::PatternRewriter& rewriter) const {
  ShapeComponentAnalysis shape_component_analysis;
  auto new_broadcast = simplifyBroadcast(
      shape_component_analysis, op.getShapes(), op.getLoc(), &rewriter);
  if (!new_broadcast) return failure();
  rewriter.replaceOp(op, {*new_broadcast});
  return success();
}