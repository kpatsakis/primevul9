  void Inspect::operator()(Attribute_Selector_Ptr s)
  {
    append_string("[");
    add_open_mapping(s);
    append_token(s->ns_name(), s);
    if (!s->matcher().empty()) {
      append_string(s->matcher());
      if (s->value() && *s->value()) {
        s->value()->perform(this);
      }
    }
    add_close_mapping(s);
    if (s->modifier() != 0) {
      append_mandatory_space();
      append_char(s->modifier());
    }
    append_string("]");
  }