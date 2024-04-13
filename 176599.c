  void Inspect::operator()(Comment_Ptr comment)
  {
    in_comment = true;
    comment->text()->perform(this);
    in_comment = false;
  }