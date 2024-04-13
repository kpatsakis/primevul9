  Supports_Condition_Obj Parser::parse_supports_condition_in_parens()
  {
    Supports_Condition_Obj interp = parse_supports_interpolation();
    if (interp != 0) return interp;

    if (!lex < exactly <'('> >()) return {};
    lex < css_whitespace >();

    Supports_Condition_Obj cond = parse_supports_condition();
    if (cond != 0) {
      if (!lex < exactly <')'> >()) error("unclosed parenthesis in @supports declaration");
    } else {
      cond = parse_supports_declaration();
      if (!lex < exactly <')'> >()) error("unclosed parenthesis in @supports declaration");
    }
    lex < css_whitespace >();
    return cond;
  }