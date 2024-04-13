  size_t removeIf(const HeaderMap::HeaderMatchPredicate& predicate) override {
    return HeaderMapImpl::removeIf(predicate);
  }