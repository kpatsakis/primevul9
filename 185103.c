void HeaderMapImpl::addViaMove(HeaderString&& key, HeaderString&& value) {
  insertByKey(std::move(key), std::move(value));
}