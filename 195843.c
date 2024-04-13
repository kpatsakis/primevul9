MatchExpression::ExpressionOptimizerFunc ExprMatchExpression::getOptimizer() const {
    return [](std::unique_ptr<MatchExpression> expression) {
        auto& exprMatchExpr = static_cast<ExprMatchExpression&>(*expression);

        // If '_expression' can be rewritten to a MatchExpression, we will return a $and node with
        // both the original ExprMatchExpression and the MatchExpression rewrite as children.
        // Exiting early prevents additional calls to optimize from performing additional rewrites
        // and adding duplicate MatchExpression sub-trees to the tree.
        if (exprMatchExpr._rewriteResult) {
            return expression;
        }

        exprMatchExpr._expression = exprMatchExpr._expression->optimize();
        exprMatchExpr._rewriteResult =
            RewriteExpr::rewrite(exprMatchExpr._expression, exprMatchExpr._expCtx->getCollator());

        if (exprMatchExpr._rewriteResult->matchExpression()) {
            auto andMatch = stdx::make_unique<AndMatchExpression>();
            andMatch->add(exprMatchExpr._rewriteResult->releaseMatchExpression().release());
            andMatch->add(expression.release());
            // Re-optimize the new AND in order to make sure that any AND children are absorbed.
            expression = MatchExpression::optimize(std::move(andMatch));
        }

        return expression;
    };
}