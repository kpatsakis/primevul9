TEST(ExprMatchTest, ExpressionOptimizeRewritesVariableDereferenceAsConstant) {
    const boost::intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto varId = expCtx->variablesParseState.defineVariable("var");
    expCtx->variables.setConstantValue(varId, Value(4));

    BSONObj expression = BSON("$expr"
                              << "$$var");
    BSONObj expressionEquiv = BSON("$expr" << BSON("$const" << 4));
    BSONObj expressionNotEquiv = BSON("$expr" << BSON("$const" << 10));

    // Create and optimize an ExprMatchExpression.
    std::unique_ptr<MatchExpression> matchExpr =
        stdx::make_unique<ExprMatchExpression>(expression.firstElement(), expCtx);
    matchExpr = MatchExpression::optimize(std::move(matchExpr));

    // We expect that the optimized 'matchExpr' is still an ExprMatchExpression.
    auto& pipelineExpr = dynamic_cast<ExprMatchExpression&>(*matchExpr);
    ASSERT_TRUE(pipelineExpr.equivalent(&pipelineExpr));

    ExprMatchExpression pipelineExprEquiv(expressionEquiv.firstElement(), expCtx);
    ASSERT_TRUE(pipelineExpr.equivalent(&pipelineExprEquiv));

    ExprMatchExpression pipelineExprNotEquiv(expressionNotEquiv.firstElement(), expCtx);
    ASSERT_FALSE(pipelineExpr.equivalent(&pipelineExprNotEquiv));
}