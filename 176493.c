  Attribute_Selector_Obj Parser::parse_attribute_selector()
  {
    ParserState p = pstate;
    if (!lex_css< attribute_name >()) error("invalid attribute name in attribute selector");
    std::string name(lexed);
    if (lex_css< re_attr_sensitive_close >()) {
      return SASS_MEMORY_NEW(Attribute_Selector, p, name, "", {}, {});
    }
    else if (lex_css< re_attr_insensitive_close >()) {
      char modifier = lexed.begin[0];
      return SASS_MEMORY_NEW(Attribute_Selector, p, name, "", {}, modifier);
    }
    if (!lex_css< alternatives< exact_match, class_match, dash_match,
                                prefix_match, suffix_match, substring_match > >()) {
      error("invalid operator in attribute selector for " + name);
    }
    std::string matcher(lexed);

    String_Obj value;
    if (lex_css< identifier >()) {
      value = SASS_MEMORY_NEW(String_Constant, p, lexed);
    }
    else if (lex_css< quoted_string >()) {
      value = parse_interpolated_chunk(lexed, true); // needed!
    }
    else {
      error("expected a string constant or identifier in attribute selector for " + name);
    }

    if (lex_css< re_attr_sensitive_close >()) {
      return SASS_MEMORY_NEW(Attribute_Selector, p, name, matcher, value, 0);
    }
    else if (lex_css< re_attr_insensitive_close >()) {
      char modifier = lexed.begin[0];
      return SASS_MEMORY_NEW(Attribute_Selector, p, name, matcher, value, modifier);
    }
    error("unterminated attribute selector for " + name);
    return {}; // to satisfy compilers (error must not return)
  }