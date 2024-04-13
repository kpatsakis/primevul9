  Expression_Obj Parser::parse_space_list()
  {
    NESTING_GUARD(nestings);
    Expression_Obj disj1 = parse_disjunction();
    // if it's a singleton, return it (don't wrap it)
    if (peek_css< space_list_terminator >(position)
    ) {
      return disj1; }

    List_Obj space_list = SASS_MEMORY_NEW(List, pstate, 2, SASS_SPACE);
    space_list->append(disj1);

    while (
      !(peek_css< space_list_terminator >(position)) &&
      peek_css< optional_css_whitespace >() != end
    ) {
      // the space is parsed implicitly?
      space_list->append(parse_disjunction());
    }
    // return the list
    return space_list;
  }