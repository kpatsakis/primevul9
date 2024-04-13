  void Inspect::operator()(Keyframe_Rule_Ptr rule)
  {
    if (rule->name()) rule->name()->perform(this);
    if (rule->block()) rule->block()->perform(this);
  }