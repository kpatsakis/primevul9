  bool Selector_List::operator==(const Expression& rhs) const
  {
    // solve the double dispatch problem by using RTTI information via dynamic cast
    if (List_Ptr_Const ls = Cast<List>(&rhs)) { return *this == *ls; }
    if (Selector_Ptr_Const ls = Cast<Selector>(&rhs)) { return *this == *ls; }
    // compare invalid (maybe we should error?)
    return false;
  }