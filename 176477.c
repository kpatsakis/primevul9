  bool number_has_zero(const std::string& parsed)
  {
    size_t L = parsed.length();
    return !( (L > 0 && parsed.substr(0, 1) == ".") ||
              (L > 1 && parsed.substr(0, 2) == "0.") ||
              (L > 1 && parsed.substr(0, 2) == "-.")  ||
              (L > 2 && parsed.substr(0, 3) == "-0.") );
  }