  void Inspect::operator()(Unary_Expression_Ptr expr)
  {
    if (expr->optype() == Unary_Expression::PLUS)       append_string("+");
    else if (expr->optype() == Unary_Expression::SLASH) append_string("/");
    else                                                append_string("-");
    expr->operand()->perform(this);
  }