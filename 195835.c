TEST(ExprMatchTest, ShallowClonedExpressionIsEquivalentToOriginal) {
    BSONObj expression = BSON("$expr" << BSON("$eq" << BSON_ARRAY("$a" << 5)));

    boost::intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    ExprMatchExpression pipelineExpr(expression.firstElement(), std::move(expCtx));
    auto shallowClone = pipelineExpr.shallowClone();
    ASSERT_TRUE(pipelineExpr.equivalent(shallowClone.get()));
}