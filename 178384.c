  Supports_Condition_Obj Parser::parse_supports_condition()
  {
    lex < css_whitespace >();
    Supports_Condition_Obj cond;
    if ((cond = parse_supports_negation())) return cond;
    if ((cond = parse_supports_operator())) return cond;
    if ((cond = parse_supports_interpolation())) return cond;
    return cond;
  }