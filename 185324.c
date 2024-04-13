void HeaderUtility::addHeaders(HeaderMap& headers, const HeaderMap& headers_to_add) {
  headers_to_add.iterate([&headers](const HeaderEntry& header) -> HeaderMap::Iterate {
    HeaderString k;
    k.setCopy(header.key().getStringView());
    HeaderString v;
    v.setCopy(header.value().getStringView());
    headers.addViaMove(std::move(k), std::move(v));
    return HeaderMap::Iterate::Continue;
  });
}