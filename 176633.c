  void Inspect::operator()(At_Root_Query_Ptr ae)
  {
    if (ae->feature()) {
      append_string("(");
      ae->feature()->perform(this);
      if (ae->value()) {
        append_colon_separator();
        ae->value()->perform(this);
      }
      append_string(")");
    }
  }