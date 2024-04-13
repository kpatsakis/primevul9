  void Inspect::operator()(Parent_Selector_Ptr p)
  {
    if (p->is_real_parent_ref()) append_string("&");
  }