    void setCollator(CollatorInterface* collator) {
        _expCtx->setCollator(collator);
        if (_matchExpression) {
            _matchExpression->setCollator(_expCtx->getCollator());
        }
    }