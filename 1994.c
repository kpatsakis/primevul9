  void runOnFunction() override {
    FuncOp func = getFunction();

    MLIRContext* ctx = &getContext();
    mlir::RewritePatternSet patterns(ctx);

    // Rewrite constraints based on the symbolic shapes.
    patterns.insert<CstrBroadcastableOpLowering>(ctx);
    // Rewrite shape.broadcast based on the symbolic shapes.
    patterns.insert<BroadcastOpLowering>(ctx);

    // Rewrite broadcasts based on the symbolic shapes if enabled.
    if (!optimize_only_constraints)
      patterns.insert<DynamicBroadcastInDimOpLowering>(ctx);

    // Add shape dialect canonicalization patterns to fold shape operations
    // after constraints are replaced with constant witness.
    for (auto op : ctx->getRegisteredOperations()) {
      if (llvm::isa<shape::ShapeDialect>(op.getDialect()))
        op.getCanonicalizationPatterns(patterns, ctx);
    }

    (void)mlir::applyPatternsAndFoldGreedily(func, std::move(patterns));
  }