void HeaderMapImpl::iterate(HeaderMap::ConstIterateCb cb) const {
  for (const HeaderEntryImpl& header : headers_) {
    if (cb(header) == HeaderMap::Iterate::Break) {
      break;
    }
  }
}