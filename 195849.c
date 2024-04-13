ExprMatchExpression::ExprMatchExpression(boost::intrusive_ptr<Expression> expr,
                                         const boost::intrusive_ptr<ExpressionContext>& expCtx)
    : MatchExpression(MatchType::EXPRESSION), _expCtx(expCtx), _expression(expr) {}