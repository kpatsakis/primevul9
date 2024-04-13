  void Inspect::operator()(Map_Ptr map)
  {
    if (output_style() == TO_SASS && map->empty()) {
      append_string("()");
      return;
    }
    if (map->empty()) return;
    if (map->is_invisible()) return;
    bool items_output = false;
    append_string("(");
    for (auto key : map->keys()) {
      if (items_output) append_comma_separator();
      key->perform(this);
      append_colon_separator();
      LOCAL_FLAG(in_space_array, true);
      LOCAL_FLAG(in_comma_array, true);
      map->at(key)->perform(this);
      items_output = true;
    }
    append_string(")");
  }