TEST(ExprMatchTest, OptimizingIsANoopWhenAlreadyOptimized) {
    const boost::intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    BSONObj expression = fromjson("{$expr: {$eq: ['$a', 4]}}");

    // Create and optimize an ExprMatchExpression.
    std::unique_ptr<MatchExpression> singlyOptimized =
        stdx::make_unique<ExprMatchExpression>(expression.firstElement(), expCtx);
    singlyOptimized = MatchExpression::optimize(std::move(singlyOptimized));

    // We expect that the optimized 'matchExpr' is now an $and.
    ASSERT(dynamic_cast<const AndMatchExpression*>(singlyOptimized.get()));

    // We expect the twice-optimized match expression to be equivalent to the once-optimized one.
    std::unique_ptr<MatchExpression> doublyOptimized =
        stdx::make_unique<ExprMatchExpression>(expression.firstElement(), expCtx);
    for (size_t i = 0; i < 2u; ++i) {
        doublyOptimized = MatchExpression::optimize(std::move(doublyOptimized));
    }
    ASSERT_TRUE(doublyOptimized->equivalent(singlyOptimized.get()));
}