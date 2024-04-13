  At_Root_Query_Obj Parser::parse_at_root_query()
  {
    if (peek< exactly<')'> >()) error("at-root feature required in at-root expression");

    if (!peek< alternatives< kwd_with_directive, kwd_without_directive > >()) {
      css_error("Invalid CSS", " after ", ": expected \"with\" or \"without\", was ");
    }

    Expression_Obj feature = parse_list();
    if (!lex_css< exactly<':'> >()) error("style declaration must contain a value");
    Expression_Obj expression = parse_list();
    List_Obj value = SASS_MEMORY_NEW(List, feature->pstate(), 1);

    if (expression->concrete_type() == Expression::LIST) {
        value = Cast<List>(expression);
    }
    else value->append(expression);

    At_Root_Query_Obj cond = SASS_MEMORY_NEW(At_Root_Query,
                                          value->pstate(),
                                          feature,
                                          value);
    if (!lex_css< exactly<')'> >()) error("unclosed parenthesis in @at-root expression");
    return cond;
  }