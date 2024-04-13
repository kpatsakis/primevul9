    bool matches(const BSONObj& doc) {
        invariant(_matchExpression);
        return _matchExpression->matchesBSON(doc);
    }