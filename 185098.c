HeaderMap::GetResult HeaderMapImpl::getAll(const LowerCaseString& key) const {
  return HeaderMap::GetResult(const_cast<HeaderMapImpl*>(this)->getExisting(key));
}