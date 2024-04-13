  For_Obj Parser::parse_for_directive()
  {
    stack.push_back(Scope::Control);
    ParserState for_source_position = pstate;
    bool root = block_stack.back()->is_root();
    lex_variable();
    std::string var(Util::normalize_underscores(lexed));
    if (!lex< kwd_from >()) error("expected 'from' keyword in @for directive");
    Expression_Obj lower_bound = parse_expression();
    bool inclusive = false;
    if (lex< kwd_through >()) inclusive = true;
    else if (lex< kwd_to >()) inclusive = false;
    else                  error("expected 'through' or 'to' keyword in @for directive");
    Expression_Obj upper_bound = parse_expression();
    Block_Obj body = parse_block(root);
    stack.pop_back();
    return SASS_MEMORY_NEW(For, for_source_position, var, lower_bound, upper_bound, body, inclusive);
  }