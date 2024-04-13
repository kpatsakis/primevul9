  Ruleset_Obj Parser::parse_ruleset(Lookahead lookahead)
  {
    NESTING_GUARD(nestings);
    // inherit is_root from parent block
    Block_Obj parent = block_stack.back();
    bool is_root = parent && parent->is_root();
    // make sure to move up the the last position
    lex < optional_css_whitespace >(false, true);
    // create the connector object (add parts later)
    Ruleset_Obj ruleset = SASS_MEMORY_NEW(Ruleset, pstate);
    // parse selector static or as schema to be evaluated later
    if (lookahead.parsable) ruleset->selector(parse_selector_list(false));
    else {
      Selector_List_Obj list = SASS_MEMORY_NEW(Selector_List, pstate);
      list->schema(parse_selector_schema(lookahead.position, false));
      ruleset->selector(list);
    }
    // then parse the inner block
    stack.push_back(Scope::Rules);
    ruleset->block(parse_block());
    stack.pop_back();
    // update for end position
    ruleset->update_pstate(pstate);
    ruleset->block()->update_pstate(pstate);
    // need this info for sanity checks
    ruleset->is_root(is_root);
    // return AST Node
    return ruleset;
  }