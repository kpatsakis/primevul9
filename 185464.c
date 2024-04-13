void HeaderMapImpl::iterateReverse(HeaderMap::ConstIterateCb cb) const {
  for (auto it = headers_.rbegin(); it != headers_.rend(); it++) {
    if (cb(*it) == HeaderMap::Iterate::Break) {
      break;
    }
  }
}