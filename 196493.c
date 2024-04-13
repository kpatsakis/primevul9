  void appendUrl(std::string& url, const std::string& str)
  {
    for (std::string::const_iterator i = str.begin();
         i != str.end(); ++i)
      appendUrl(url, *i);
  }