void Parser::shift(int objNum) {
  if (inlineImg > 0) {
    if (inlineImg < 2) {
      ++inlineImg;
    } else {
      // in a damaged content stream, if 'ID' shows up in the middle
      // of a dictionary, we need to reset
      inlineImg = 0;
    }
  } else if (buf2.isCmd("ID")) {
    lexer->skipChar();		// skip char after 'ID' command
    inlineImg = 1;
  }
  buf1 = std::move(buf2);
  if (inlineImg > 0)		// don't buffer inline image data
    buf2.setToNull();
  else {
    buf2 = lexer->getObj(objNum);
  }
}