  If_Obj Parser::parse_if_directive(bool else_if)
  {
    stack.push_back(Scope::Control);
    ParserState if_source_position = pstate;
    bool root = block_stack.back()->is_root();
    Expression_Obj predicate = parse_list();
    Block_Obj block = parse_block(root);
    Block_Obj alternative;

    // only throw away comment if we parse a case
    // we want all other comments to be parsed
    if (lex_css< elseif_directive >()) {
      alternative = SASS_MEMORY_NEW(Block, pstate);
      alternative->append(parse_if_directive(true));
    }
    else if (lex_css< kwd_else_directive >()) {
      alternative = parse_block(root);
    }
    stack.pop_back();
    return SASS_MEMORY_NEW(If, if_source_position, predicate, block, alternative);
  }