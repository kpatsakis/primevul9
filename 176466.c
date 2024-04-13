  Expression_Obj Parser::parse_map()
  {
    NESTING_GUARD(nestings);
    Expression_Obj key = parse_list();
    List_Obj map = SASS_MEMORY_NEW(List, pstate, 0, SASS_HASH);

    // it's not a map so return the lexed value as a list value
    if (!lex_css< exactly<':'> >())
    { return key; }

    List_Obj l = Cast<List>(key);
    if (l && l->separator() == SASS_COMMA) {
      css_error("Invalid CSS", " after ", ": expected \")\", was ");
    }

    Expression_Obj value = parse_space_list();

    map->append(key);
    map->append(value);

    while (lex_css< exactly<','> >())
    {
      // allow trailing commas - #495
      if (peek_css< exactly<')'> >(position))
      { break; }

      key = parse_space_list();

      if (!(lex< exactly<':'> >()))
      { css_error("Invalid CSS", " after ", ": expected \":\", was "); }

      value = parse_space_list();

      map->append(key);
      map->append(value);
    }

    ParserState ps = map->pstate();
    ps.offset = pstate - ps + pstate.offset;
    map->pstate(ps);

    return map;
  }