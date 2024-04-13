  static std::string getIpv4Loopback() {
#ifdef __APPLE__
    return "127.0.0.1";
#else
    return "127.0.0.9";
#endif
  }