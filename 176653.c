  void Inspect::operator()(Bubble_Ptr bubble)
  {
    append_indentation();
    append_token("::BUBBLE", bubble);
    append_scope_opener();
    bubble->node()->perform(this);
    append_scope_closer();
  }