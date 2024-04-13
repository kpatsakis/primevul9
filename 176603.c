  void Inspect::operator()(Supports_Operator_Ptr so)
  {

    if (so->needs_parens(so->left())) append_string("(");
    so->left()->perform(this);
    if (so->needs_parens(so->left())) append_string(")");

    if (so->operand() == Supports_Operator::AND) {
      append_mandatory_space();
      append_token("and", so);
      append_mandatory_space();
    } else if (so->operand() == Supports_Operator::OR) {
      append_mandatory_space();
      append_token("or", so);
      append_mandatory_space();
    }

    if (so->needs_parens(so->right())) append_string("(");
    so->right()->perform(this);
    if (so->needs_parens(so->right())) append_string(")");
  }