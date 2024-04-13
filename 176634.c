  void Inspect::operator()(Argument_Ptr a)
  {
    if (!a->name().empty()) {
      append_token(a->name(), a);
      append_colon_separator();
    }
    if (!a->value()) return;
    // Special case: argument nulls can be ignored
    if (a->value()->concrete_type() == Expression::NULL_VAL) {
      return;
    }
    if (a->value()->concrete_type() == Expression::STRING) {
      String_Constant_Ptr s = Cast<String_Constant>(a->value());
      if (s) s->perform(this);
    } else {
      a->value()->perform(this);
    }
    if (a->is_rest_argument()) {
      append_string("...");
    }
  }