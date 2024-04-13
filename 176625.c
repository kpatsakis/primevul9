  void Inspect::operator()(At_Root_Block_Ptr at_root_block)
  {
    append_indentation();
    append_token("@at-root ", at_root_block);
    append_mandatory_space();
    if(at_root_block->expression()) at_root_block->expression()->perform(this);
    if(at_root_block->block()) at_root_block->block()->perform(this);
  }