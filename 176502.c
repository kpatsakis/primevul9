  Expression_Obj Parser::lex_almost_any_value_token()
  {
    Expression_Obj rv;
    if (*position == 0) return {};
    if ((rv = lex_almost_any_value_chars())) return rv;
    // if ((rv = lex_block_comment())) return rv;
    // if ((rv = lex_single_line_comment())) return rv;
    if ((rv = lex_interp_string())) return rv;
    if ((rv = lex_interp_uri())) return rv;
    if ((rv = lex_interpolation())) return rv;
     if (lex< alternatives< hex, hex0 > >())
    { return lexed_hex_color(lexed); }
   return rv;
  }