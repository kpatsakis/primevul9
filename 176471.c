  Block_Obj Parser::parse_css_block(bool is_root)
  {

    // parse comments before block
    // lex < optional_css_comments >();

    // lex mandatory opener or error out
    if (!lex_css < exactly<'{'> >()) {
      css_error("Invalid CSS", " after ", ": expected \"{\", was ");
    }
    // create new block and push to the selector stack
    Block_Obj block = SASS_MEMORY_NEW(Block, pstate, 0, is_root);
    block_stack.push_back(block);

    if (!parse_block_nodes(is_root)) css_error("Invalid CSS", " after ", ": expected \"}\", was ");

    if (!lex_css < exactly<'}'> >()) {
      css_error("Invalid CSS", " after ", ": expected \"}\", was ");
    }

    // update for end position
    // this seems to be done somewhere else
    // but that fixed selector schema issue
    // block->update_pstate(pstate);

    // parse comments after block
    // lex < optional_css_comments >();

    block_stack.pop_back();

    return block;
  }