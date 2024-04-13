bool ExprMatchExpression::equivalent(const MatchExpression* other) const {
    if (other->matchType() != matchType()) {
        return false;
    }

    const ExprMatchExpression* realOther = static_cast<const ExprMatchExpression*>(other);

    if (!CollatorInterface::collatorsMatch(_expCtx->getCollator(),
                                           realOther->_expCtx->getCollator())) {
        return false;
    }

    // TODO SERVER-30982: Add mechanism to allow for checking Expression equivalency.
    return ValueComparator().evaluate(_expression->serialize(false) ==
                                      realOther->_expression->serialize(false));
}