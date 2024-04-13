  Wrapped_Selector_Obj Parser::parse_negated_selector()
  {
    lex< pseudo_not >();
    std::string name(lexed);
    ParserState nsource_position = pstate;
    Selector_List_Obj negated = parse_selector_list(true);
    if (!lex< exactly<')'> >()) {
      error("negated selector is missing ')'");
    }
    name.erase(name.size() - 1);
    return SASS_MEMORY_NEW(Wrapped_Selector, nsource_position, name, negated);
  }