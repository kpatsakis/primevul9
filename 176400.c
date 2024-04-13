  String_Obj Parser::parse_ie_keyword_arg()
  {
    String_Schema_Obj kwd_arg = SASS_MEMORY_NEW(String_Schema, pstate, 3);
    if (lex< variable >()) {
      kwd_arg->append(SASS_MEMORY_NEW(Variable, pstate, Util::normalize_underscores(lexed)));
    } else {
      lex< alternatives< identifier_schema, identifier > >();
      kwd_arg->append(SASS_MEMORY_NEW(String_Constant, pstate, lexed));
    }
    lex< exactly<'='> >();
    kwd_arg->append(SASS_MEMORY_NEW(String_Constant, pstate, lexed));
    if (peek< variable >()) kwd_arg->append(parse_list());
    else if (lex< number >()) {
      std::string parsed(lexed);
      Util::normalize_decimals(parsed);
      kwd_arg->append(lexed_number(parsed));
    }
    else if (peek < ie_keyword_arg_value >()) { kwd_arg->append(parse_list()); }
    return kwd_arg;
  }