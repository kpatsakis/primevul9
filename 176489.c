  Expression_Obj Parser::parse_disjunction()
  {
    NESTING_GUARD(nestings);
    advanceToNextToken();
    ParserState state(pstate);
    // parse the left hand side conjunction
    Expression_Obj conj = parse_conjunction();
    // parse multiple right hand sides
    std::vector<Expression_Obj> operands;
    while (lex_css< kwd_or >())
      operands.push_back(parse_conjunction());
    // if it's a singleton, return it directly
    if (operands.size() == 0) return conj;
    // fold all operands into one binary expression
    Expression_Obj ex = fold_operands(conj, operands, { Sass_OP::OR });
    state.offset = pstate - state + pstate.offset;
    ex->pstate(state);
    return ex;
  }