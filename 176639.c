  void Inspect::operator()(Supports_Interpolation_Ptr sd)
  {
    sd->value()->perform(this);
  }