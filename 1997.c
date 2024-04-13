std::unique_ptr<FunctionPass> CreateSymbolicShapeOptimizationPass(
    bool constraints_only) {
  return std::make_unique<SymbolicShapeOptimizationPass>(constraints_only);
}