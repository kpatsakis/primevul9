  Expression_Obj Parser::fold_operands(Expression_Obj base, std::vector<Expression_Obj>& operands, Operand op)
  {
    for (size_t i = 0, S = operands.size(); i < S; ++i) {
      base = SASS_MEMORY_NEW(Binary_Expression, base->pstate(), op, base, operands[i]);
    }
    return base;
  }