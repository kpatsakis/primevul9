  Lookahead Parser::lookahead_for_selector(const char* start)
  {
    // init result struct
    Lookahead rv = Lookahead();
    // get start position
    const char* p = start ? start : position;
    // match in one big "regex"
    rv.error = p;
    if (const char* q =
      peek <
        re_selector_list
      >(p)
    ) {
      bool could_be_property = peek< sequence< exactly<'-'>, exactly<'-'> > >(p) != 0;
      bool could_be_escaped = false;
      while (p < q) {
        // did we have interpolations?
        if (*p == '#' && *(p+1) == '{') {
          rv.has_interpolants = true;
          p = q; break;
        }
        // A property that's ambiguous with a nested selector is interpreted as a
        // custom property.
        if (*p == ':' && !could_be_escaped) {
          rv.is_custom_property = could_be_property || p+1 == q || peek< space >(p+1);
        }
        could_be_escaped = *p == '\\';
        ++ p;
      }
      // store anyway  }


      // ToDo: remove
      rv.error = q;
      rv.position = q;
      // check expected opening bracket
      // only after successfull matching
      if (peek < exactly<'{'> >(q)) rv.found = q;
      // else if (peek < end_of_file >(q)) rv.found = q;
      else if (peek < exactly<'('> >(q)) rv.found = q;
      // else if (peek < exactly<';'> >(q)) rv.found = q;
      // else if (peek < exactly<'}'> >(q)) rv.found = q;
      if (rv.found || *p == 0) rv.error = 0;
    }

    rv.parsable = ! rv.has_interpolants;

    // return result
    return rv;

  }