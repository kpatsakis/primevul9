  void Inspect::operator()(String_Constant_Ptr s)
  {
    append_token(s->value(), s);
  }