void HeaderMapImpl::dumpState(std::ostream& os, int indent_level) const {
  iterate([&os,
           spaces = spacesForLevel(indent_level)](const HeaderEntry& header) -> HeaderMap::Iterate {
    os << spaces << "'" << header.key().getStringView() << "', '" << header.value().getStringView()
       << "'\n";
    return HeaderMap::Iterate::Continue;
  });
}