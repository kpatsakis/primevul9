  void Inspect::operator()(Wrapped_Selector_Ptr s)
  {
    if (s->name() == " ") {
      append_string("");
    } else {
      bool was = in_wrapped;
      in_wrapped = true;
      append_token(s->name(), s);
      append_string("(");
      bool was_comma_array = in_comma_array;
      in_comma_array = false;
      s->selector()->perform(this);
      in_comma_array = was_comma_array;
      append_string(")");
      in_wrapped = was;
    }
  }