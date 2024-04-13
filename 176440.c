  void Parser::parse_charset_directive()
  {
    lex <
      sequence <
        quoted_string,
        optional_spaces,
        exactly <';'>
      >
    >();
  }