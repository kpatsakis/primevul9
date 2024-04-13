  Expression_Obj Parser::parse_list(bool delayed)
  {
    NESTING_GUARD(nestings);
    return parse_comma_list(delayed);
  }