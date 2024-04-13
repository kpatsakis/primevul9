  String_Obj Parser::parse_url_function_string()
  {
    std::string prefix("");
    if (lex< uri_prefix >()) {
      prefix = std::string(lexed);
    }

    lex < optional_spaces >();
    String_Obj url_string = parse_url_function_argument();

    std::string suffix("");
    if (lex< real_uri_suffix >()) {
      suffix = std::string(lexed);
    }

    std::string uri("");
    if (url_string) {
      uri = url_string->to_string({ NESTED, 5 });
    }

    if (String_Schema* schema = Cast<String_Schema>(url_string)) {
      String_Schema_Obj res = SASS_MEMORY_NEW(String_Schema, pstate);
      res->append(SASS_MEMORY_NEW(String_Constant, pstate, prefix));
      res->append(schema);
      res->append(SASS_MEMORY_NEW(String_Constant, pstate, suffix));
      return res;
    } else {
      std::string res = prefix + uri + suffix;
      return SASS_MEMORY_NEW(String_Constant, pstate, res);
    }
  }