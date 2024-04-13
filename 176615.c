  void Inspect::operator()(Ruleset_Ptr ruleset)
  {
    if (ruleset->selector()) {
      ruleset->selector()->perform(this);
    }
    if (ruleset->block()) {
      ruleset->block()->perform(this);
    }
  }