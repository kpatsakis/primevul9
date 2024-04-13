  void Inspect::operator()(Boolean_Ptr b)
  {
    // output the final token
    append_token(b->value() ? "true" : "false", b);
  }