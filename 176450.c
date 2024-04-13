  void Parser::parse_block_comments()
  {
    Block_Obj block = block_stack.back();

    while (lex< block_comment >()) {
      bool is_important = lexed.begin[2] == '!';
      // flag on second param is to skip loosely over comments
      String_Obj contents = parse_interpolated_chunk(lexed, true, false);
      block->append(SASS_MEMORY_NEW(Comment, pstate, contents, is_important));
    }
  }