std::unique_ptr<MatchExpression> ExprMatchExpression::shallowClone() const {
    // TODO SERVER-31003: Replace Expression clone via serialization with Expression::clone().
    BSONObjBuilder bob;
    bob << "" << _expression->serialize(false);
    boost::intrusive_ptr<Expression> clonedExpr =
        Expression::parseOperand(_expCtx, bob.obj().firstElement(), _expCtx->variablesParseState);

    auto clone = stdx::make_unique<ExprMatchExpression>(std::move(clonedExpr), _expCtx);
    if (_rewriteResult) {
        clone->_rewriteResult = _rewriteResult->clone();
    }
    return std::move(clone);
}