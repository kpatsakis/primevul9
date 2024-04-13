  Each_Obj Parser::parse_each_directive()
  {
    stack.push_back(Scope::Control);
    ParserState each_source_position = pstate;
    bool root = block_stack.back()->is_root();
    std::vector<std::string> vars;
    lex_variable();
    vars.push_back(Util::normalize_underscores(lexed));
    while (lex< exactly<','> >()) {
      if (!lex< variable >()) error("@each directive requires an iteration variable");
      vars.push_back(Util::normalize_underscores(lexed));
    }
    if (!lex< kwd_in >()) error("expected 'in' keyword in @each directive");
    Expression_Obj list = parse_list();
    Block_Obj body = parse_block(root);
    stack.pop_back();
    return SASS_MEMORY_NEW(Each, each_source_position, vars, list, body);
  }