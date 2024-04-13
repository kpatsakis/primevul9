  Supports_Condition_Obj Parser::parse_supports_interpolation()
  {
    if (!lex < interpolant >()) return 0;

    String_Obj interp = parse_interpolated_chunk(lexed);
    if (!interp) return 0;

    return SASS_MEMORY_NEW(Supports_Interpolation, pstate, interp);
  }