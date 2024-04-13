  Expression_Obj Parser::parse_operators()
  {
    NESTING_GUARD(nestings);
    advanceToNextToken();
    ParserState state(pstate);
    Expression_Obj factor = parse_factor();
    // if it's a singleton, return it (don't wrap it)
    std::vector<Expression_Obj> operands; // factors
    std::vector<Operand> operators; // ops
    // lex operations to apply to lhs
    const char* left_ws = peek < css_comments >();
    while (lex_css< class_char< static_ops > >()) {
      const char* right_ws = peek < css_comments >();
      switch(*lexed.begin) {
        case '*': operators.push_back({ Sass_OP::MUL, left_ws != 0, right_ws != 0 }); break;
        case '/': operators.push_back({ Sass_OP::DIV, left_ws != 0, right_ws != 0 }); break;
        case '%': operators.push_back({ Sass_OP::MOD, left_ws != 0, right_ws != 0 }); break;
        default: throw std::runtime_error("unknown static op parsed");
      }
      operands.push_back(parse_factor());
      left_ws = peek < css_comments >();
    }
    // operands and operators to binary expression
    Expression_Obj ex = fold_operands(factor, operands, operators);
    state.offset = pstate - state + pstate.offset;
    ex->pstate(state);
    return ex;
  }