  void Inspect::operator()(Selector_Schema_Ptr s)
  {
    s->contents()->perform(this);
  }