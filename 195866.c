    void setVariable(StringData name, Value val) {
        auto varId = _expCtx->variablesParseState.defineVariable(name);
        _expCtx->variables.setValue(varId, val);
    }