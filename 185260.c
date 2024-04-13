void HeaderMapImpl::copyFrom(HeaderMap& lhs, const HeaderMap& header_map) {
  header_map.iterate([&lhs](const HeaderEntry& header) -> HeaderMap::Iterate {
    // TODO(mattklein123) PERF: Avoid copying here if not necessary.
    HeaderString key_string;
    key_string.setCopy(header.key().getStringView());
    HeaderString value_string;
    value_string.setCopy(header.value().getStringView());

    lhs.addViaMove(std::move(key_string), std::move(value_string));
    return HeaderMap::Iterate::Continue;
  });
}