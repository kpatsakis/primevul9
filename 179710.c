  String_Schema_Obj Parser::parse_almost_any_value()
  {

    String_Schema_Obj schema = SASS_MEMORY_NEW(String_Schema, pstate);
    if (*position == 0) return 0;
    lex < spaces >(false);
    Expression_Obj token = lex_almost_any_value_token();
    if (!token) return 0;
    schema->append(token);
    if (*position == 0) {
      schema->rtrim();
      return schema.detach();
    }

    while ((token = lex_almost_any_value_token())) {
      schema->append(token);
    }

    lex < css_whitespace >();

    schema->rtrim();

    return schema.detach();
  }