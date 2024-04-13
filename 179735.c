  String_Schema_Obj Parser::parse_css_variable_value(bool top_level)
  {
    String_Schema_Obj schema = SASS_MEMORY_NEW(String_Schema, pstate);
    String_Schema_Obj tok;
    if (!(tok = parse_css_variable_value_token(top_level))) {
      return NULL;
    }

    schema->concat(tok);
    while ((tok = parse_css_variable_value_token(top_level))) {
      schema->concat(tok);
    }

    return schema.detach();
  }