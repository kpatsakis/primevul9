  Expression_Obj Parser::parse_expression()
  {
    NESTING_GUARD(nestings);
    advanceToNextToken();
    ParserState state(pstate);
    // parses multiple add and subtract operations
    // NOTE: make sure that identifiers starting with
    // NOTE: dashes do NOT count as subtract operation
    Expression_Obj lhs = parse_operators();
    // if it's a singleton, return it (don't wrap it)
    if (!(peek_css< exactly<'+'> >(position) ||
          // condition is a bit misterious, but some combinations should not be counted as operations
          (peek< no_spaces >(position) && peek< sequence< negate< unsigned_number >, exactly<'-'>, negate< space > > >(position)) ||
          (peek< sequence< negate< unsigned_number >, exactly<'-'>, negate< unsigned_number > > >(position))) ||
          peek< sequence < zero_plus < exactly <'-' > >, identifier > >(position))
    { return lhs; }

    std::vector<Expression_Obj> operands;
    std::vector<Operand> operators;
    bool left_ws = peek < css_comments >() != NULL;
    while (
      lex_css< exactly<'+'> >() ||

      (
      ! peek_css< sequence < zero_plus < exactly <'-' > >, identifier > >(position)
      && lex_css< sequence< negate< digit >, exactly<'-'> > >()
      )

    ) {

      bool right_ws = peek < css_comments >() != NULL;
      operators.push_back({ lexed.to_string() == "+" ? Sass_OP::ADD : Sass_OP::SUB, left_ws, right_ws });
      operands.push_back(parse_operators());
      left_ws = peek < css_comments >() != NULL;
    }

    if (operands.size() == 0) return lhs;
    Expression_Obj ex = fold_operands(lhs, operands, operators);
    state.offset = pstate - state + pstate.offset;
    ex->pstate(state);
    return ex;
  }