  Parser Parser::from_c_str(const char* beg, Context& ctx, Backtraces traces, ParserState pstate, const char* source, bool allow_parent)
  {
    pstate.offset.column = 0;
    pstate.offset.line = 0;
    Parser p(ctx, pstate, traces, allow_parent);
    p.source   = source ? source : beg;
    p.position = beg ? beg : p.source;
    p.end      = p.position + strlen(p.position);
    Block_Obj root = SASS_MEMORY_NEW(Block, pstate);
    p.block_stack.push_back(root);
    root->is_root(true);
    return p;
  }