   void Parser::advanceToNextToken() {
      lex < css_comments >(false);
      // advance to position
      pstate += pstate.offset;
      pstate.offset.column = 0;
      pstate.offset.line = 0;
    }