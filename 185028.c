  void dumpState(std::ostream& os, int indent_level = 0) const override {
    header_map_->dumpState(os, indent_level);
  }