  Supports_Condition_Obj Parser::parse_supports_declaration()
  {
    Supports_Condition_Ptr cond;
    // parse something declaration like
    Expression_Obj feature = parse_expression();
    Expression_Obj expression = 0;
    if (lex_css< exactly<':'> >()) {
      expression = parse_list(DELAYED);
    }
    if (!feature || !expression) error("@supports condition expected declaration");
    cond = SASS_MEMORY_NEW(Supports_Declaration,
                     feature->pstate(),
                     feature,
                     expression);
    // ToDo: maybe we need an additional error condition?
    return cond;
  }