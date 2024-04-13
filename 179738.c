  Value_Ptr Parser::lexed_hex_color(const ParserState& pstate, const std::string& parsed)
  {
    Color_Ptr color = NULL;
    if (parsed[0] != '#') {
      return SASS_MEMORY_NEW(String_Quoted, pstate, parsed);
    }
    // chop off the '#'
    std::string hext(parsed.substr(1));
    if (parsed.length() == 4) {
      std::string r(2, parsed[1]);
      std::string g(2, parsed[2]);
      std::string b(2, parsed[3]);
      color = SASS_MEMORY_NEW(Color,
                               pstate,
                               static_cast<double>(strtol(r.c_str(), NULL, 16)),
                               static_cast<double>(strtol(g.c_str(), NULL, 16)),
                               static_cast<double>(strtol(b.c_str(), NULL, 16)),
                               1, // alpha channel
                               parsed);
    }
    else if (parsed.length() == 7) {
      std::string r(parsed.substr(1,2));
      std::string g(parsed.substr(3,2));
      std::string b(parsed.substr(5,2));
      color = SASS_MEMORY_NEW(Color,
                               pstate,
                               static_cast<double>(strtol(r.c_str(), NULL, 16)),
                               static_cast<double>(strtol(g.c_str(), NULL, 16)),
                               static_cast<double>(strtol(b.c_str(), NULL, 16)),
                               1, // alpha channel
                               parsed);
    }
    else if (parsed.length() == 9) {
      std::string r(parsed.substr(1,2));
      std::string g(parsed.substr(3,2));
      std::string b(parsed.substr(5,2));
      std::string a(parsed.substr(7,2));
      color = SASS_MEMORY_NEW(Color,
                               pstate,
                               static_cast<double>(strtol(r.c_str(), NULL, 16)),
                               static_cast<double>(strtol(g.c_str(), NULL, 16)),
                               static_cast<double>(strtol(b.c_str(), NULL, 16)),
                               static_cast<double>(strtol(a.c_str(), NULL, 16)) / 255,
                               parsed);
    }
    color->is_interpolant(false);
    color->is_delayed(false);
    return color;
  }