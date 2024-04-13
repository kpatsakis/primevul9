  void Inspect::operator()(String_Quoted_Ptr s)
  {
    if (const char q = s->quote_mark()) {
      append_token(quote(s->value(), q), s);
    } else {
      append_token(s->value(), s);
    }
  }