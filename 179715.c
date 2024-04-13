  Expression_Obj Parser::lex_interpolation()
  {
    if (lex < interpolant >(true) != NULL) {
      return parse_interpolated_chunk(lexed, true);
    }
    return 0;
  }