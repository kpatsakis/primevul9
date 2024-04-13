  Expression_Obj Parser::parse_bracket_list()
  {
    NESTING_GUARD(nestings);
    // check if we have an empty list
    // return the empty list as such
    if (peek_css< list_terminator >(position))
    {
      // return an empty list (nothing to delay)
      return SASS_MEMORY_NEW(List, pstate, 0, SASS_SPACE, false, true);
    }

    bool has_paren = peek_css< exactly<'('> >() != NULL;

    // now try to parse a space list
    Expression_Obj list = parse_space_list();
    // if it's a singleton, return it (don't wrap it)
    if (!peek_css< exactly<','> >(position)) {
      List_Obj l = Cast<List>(list);
      if (!l || l->is_bracketed() || has_paren) {
        List_Obj bracketed_list = SASS_MEMORY_NEW(List, pstate, 1, SASS_SPACE, false, true);
        bracketed_list->append(list);
        return bracketed_list;
      }
      l->is_bracketed(true);
      return l;
    }

    // if we got so far, we actually do have a comma list
    List_Obj bracketed_list = SASS_MEMORY_NEW(List, pstate, 2, SASS_COMMA, false, true);
    // wrap the first expression
    bracketed_list->append(list);

    while (lex_css< exactly<','> >())
    {
      // check for abort condition
      if (peek_css< list_terminator >(position)
      ) { break; }
      // otherwise add another expression
      bracketed_list->append(parse_space_list());
    }
    // return the list
    return bracketed_list;
  }