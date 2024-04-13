  void appendUrl(std::string& url, char ch)
  {
    static const char hex[] = "0123456789ABCDEF";
    if (ch > 32 && ch < 127 && ch != '%' && ch != '+' && ch != '=' && ch != '&')
      url += ch;
    else if (ch == ' ')
      url += '+';
    else
    {
      url += '%';
      char hi = (ch >> 4) & 0x0f;
      char lo = ch & 0x0f;
      url += hex[static_cast<int>(hi)];
      url += hex[static_cast<int>(lo)];
    }
  }