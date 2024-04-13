HeaderMapIterator::HeaderMapIterator(HeaderMapWrapper& parent) : parent_(parent) {
  entries_.reserve(parent_.headers_.size());
  parent_.headers_.iterate([this](const Http::HeaderEntry& header) -> Http::HeaderMap::Iterate {
    entries_.push_back(&header);
    return Http::HeaderMap::Iterate::Continue;
  });
}