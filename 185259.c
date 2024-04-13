  HeaderMap::ConstIterateCb asIterateCb() {
    return [this](const Http::HeaderEntry& header) -> HeaderMap::Iterate {
      Call(std::string(header.key().getStringView()), std::string(header.value().getStringView()));
      return HeaderMap::Iterate::Continue;
    };
  }