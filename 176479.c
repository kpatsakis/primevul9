  Expression_Obj Parser::parse_conjunction()
  {
    NESTING_GUARD(nestings);
    advanceToNextToken();
    ParserState state(pstate);
    // parse the left hand side relation
    Expression_Obj rel = parse_relation();
    // parse multiple right hand sides
    std::vector<Expression_Obj> operands;
    while (lex_css< kwd_and >()) {
      operands.push_back(parse_relation());
    }
    // if it's a singleton, return it directly
    if (operands.size() == 0) return rel;
    // fold all operands into one binary expression
    Expression_Obj ex = fold_operands(rel, operands, { Sass_OP::AND });
    state.offset = pstate - state + pstate.offset;
    ex->pstate(state);
    return ex;
  }