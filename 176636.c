  void Inspect::operator()(Pseudo_Selector_Ptr s)
  {
    append_token(s->ns_name(), s);
    if (s->expression()) {
      append_string("(");
      s->expression()->perform(this);
      append_string(")");
    }
  }