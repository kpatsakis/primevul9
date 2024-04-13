  Lookahead Parser::lookahead_for_include(const char* start)
  {
    // we actually just lookahead for a selector
    Lookahead rv = lookahead_for_selector(start);
    // but the "found" rules are different
    if (const char* p = rv.position) {
      // check for additional abort condition
      if (peek < exactly<';'> >(p)) rv.found = p;
      else if (peek < exactly<'}'> >(p)) rv.found = p;
    }
    // return result
    return rv;
  }