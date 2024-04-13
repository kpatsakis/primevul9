  void Inspect::operator()(While_Ptr loop)
  {
    append_indentation();
    append_token("@while", loop);
    append_mandatory_space();
    loop->predicate()->perform(this);
    loop->block()->perform(this);
  }