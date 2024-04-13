  Media_Query_Expression_Obj Parser::parse_media_expression()
  {
    if (lex < identifier_schema >()) {
      String_Obj ss = parse_identifier_schema();
      return SASS_MEMORY_NEW(Media_Query_Expression, pstate, ss, {}, true);
    }
    if (!lex_css< exactly<'('> >()) {
      error("media query expression must begin with '('");
    }
    Expression_Obj feature;
    if (peek_css< exactly<')'> >()) {
      error("media feature required in media query expression");
    }
    feature = parse_expression();
    Expression_Obj expression;
    if (lex_css< exactly<':'> >()) {
      expression = parse_list(DELAYED);
    }
    if (!lex_css< exactly<')'> >()) {
      error("unclosed parenthesis in media query expression");
    }
    return SASS_MEMORY_NEW(Media_Query_Expression, feature->pstate(), feature, expression);
  }