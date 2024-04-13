  Parser Parser::from_token(Token t, Context& ctx, Backtraces traces, ParserState pstate, const char* source)
  {
    Parser p(ctx, pstate, traces);
    p.source   = source ? source : t.begin;
    p.position = t.begin ? t.begin : p.source;
    p.end      = t.end ? t.end : p.position + strlen(p.position);
    Block_Obj root = SASS_MEMORY_NEW(Block, pstate);
    p.block_stack.push_back(root);
    root->is_root(true);
    return p;
  }