  void Inspect::operator()(Definition_Ptr def)
  {
    append_indentation();
    if (def->type() == Definition::MIXIN) {
      append_token("@mixin", def);
      append_mandatory_space();
    } else {
      append_token("@function", def);
      append_mandatory_space();
    }
    append_string(def->name());
    def->parameters()->perform(this);
    def->block()->perform(this);
  }