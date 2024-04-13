  Supports_Condition_Obj Parser::parse_supports_condition_in_parens(bool parens_required)
  {
    Supports_Condition_Obj interp = parse_supports_interpolation();
    if (interp != 0) return interp;

    if (!lex < exactly <'('> >()) {
      if (parens_required) {
        css_error("Invalid CSS", " after ", ": expected @supports condition (e.g. (display: flexbox)), was ", /*trim=*/false);
      } else {
        return {};
      }
    }
    lex < css_whitespace >();

    Supports_Condition_Obj cond = parse_supports_condition(/*top_level=*/false);
    if (cond.isNull()) cond = parse_supports_declaration();
    if (!lex < exactly <')'> >()) error("unclosed parenthesis in @supports declaration");

    lex < css_whitespace >();
    return cond;
  }