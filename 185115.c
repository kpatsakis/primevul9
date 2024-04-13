  const HeaderEntry& hostHeaderEntry(const std::string& host_value, bool set_connect = false) {
    headers_.setHost(host_value);
    if (set_connect) {
      headers_.setMethod(Http::Headers::get().MethodValues.Connect);
    }
    return *headers_.Host();
  }