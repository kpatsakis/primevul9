  Supports_Condition_Obj Parser::parse_supports_negation()
  {
    if (!lex < kwd_not >()) return {};
    Supports_Condition_Obj cond = parse_supports_condition_in_parens(/*parens_required=*/true);
    return SASS_MEMORY_NEW(Supports_Negation, pstate, cond);
  }