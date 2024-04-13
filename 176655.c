  void Inspect::operator()(Media_Query_Expression_Ptr mqe)
  {
    if (mqe->is_interpolated()) {
      mqe->feature()->perform(this);
    }
    else {
      append_string("(");
      mqe->feature()->perform(this);
      if (mqe->value()) {
        append_string(": "); // verified
        mqe->value()->perform(this);
      }
      append_string(")");
    }
  }