  bool Parser::parse_number_prefix()
  {
    bool positive = true;
    while(true) {
      if (lex < block_comment >()) continue;
      if (lex < number_prefix >()) continue;
      if (lex < exactly < '-' > >()) {
        positive = !positive;
        continue;
      }
      break;
    }
    return positive;
  }