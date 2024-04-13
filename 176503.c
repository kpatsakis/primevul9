  String_Obj Parser::parse_string()
  {
    return parse_interpolated_chunk(Token(lexed));
  }