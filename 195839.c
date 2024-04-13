void ExprMatchExpression::_doSetCollator(const CollatorInterface* collator) {
    _expCtx->setCollator(collator);

    if (_rewriteResult && _rewriteResult->matchExpression()) {
        _rewriteResult->matchExpression()->setCollator(collator);
    }
}