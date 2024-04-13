TEST(ExprMatchTest, OptimizingExprAbsorbsAndOfAnd) {
    BSONObj exprBson = fromjson("{$expr: {$and: [{$eq: ['$a', 1]}, {$eq: ['$b', 2]}]}}");

    boost::intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    auto matchExpr =
        std::make_unique<ExprMatchExpression>(exprBson.firstElement(), std::move(expCtx));
    auto optimized = MatchExpression::optimize(std::move(matchExpr));

    // The optimized match expression should not have and AND children of AND nodes. This should be
    // collapsed during optimization.
    BSONObj serialized;
    {
        BSONObjBuilder builder;
        optimized->serialize(&builder);
        serialized = builder.obj();
    }

    BSONObj expectedSerialization = fromjson(
        "{$and: [{$expr: {$and: [{$eq: ['$a', {$const: 1}]}, {$eq: ['$b', {$const: 2}]}]}},"
        "{a: {$_internalExprEq: 1}}, {b: {$_internalExprEq: 2}}]}");
    ASSERT_BSONOBJ_EQ(serialized, expectedSerialization);
}