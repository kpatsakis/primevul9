Parser::Parser(XRef *xrefA, Lexer *lexerA, GBool allowStreamsA) {
  xref = xrefA;
  lexer = lexerA;
  inlineImg = 0;
  allowStreams = allowStreamsA;
  buf1 = lexer->getObj();
  buf2 = lexer->getObj();
}