ExprMatchExpression::ExprMatchExpression(BSONElement elem,
                                         const boost::intrusive_ptr<ExpressionContext>& expCtx)
    : ExprMatchExpression(Expression::parseOperand(expCtx, elem, expCtx->variablesParseState),
                          expCtx) {}