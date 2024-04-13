  At_Root_Block_Obj Parser::parse_at_root_block()
  {
    stack.push_back(Scope::AtRoot);
    ParserState at_source_position = pstate;
    Block_Obj body;
    At_Root_Query_Obj expr;
    Lookahead lookahead_result;
    if (lex_css< exactly<'('> >()) {
      expr = parse_at_root_query();
    }
    if (peek_css < exactly<'{'> >()) {
      lex <optional_spaces>();
      body = parse_block(true);
    }
    else if ((lookahead_result = lookahead_for_selector(position)).found) {
      Ruleset_Obj r = parse_ruleset(lookahead_result);
      body = SASS_MEMORY_NEW(Block, r->pstate(), 1, true);
      body->append(r);
    }
    At_Root_Block_Obj at_root = SASS_MEMORY_NEW(At_Root_Block, at_source_position, body);
    if (!expr.isNull()) at_root->expression(expr);
    stack.pop_back();
    return at_root;
  }