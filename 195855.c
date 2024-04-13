bool ExprMatchExpression::matches(const MatchableDocument* doc, MatchDetails* details) const {
    if (_rewriteResult && _rewriteResult->matchExpression() &&
        !_rewriteResult->matchExpression()->matches(doc, details)) {
        return false;
    }

    Document document(doc->toBSON());
    auto value = _expression->evaluate(document);
    return value.coerceToBool();
}