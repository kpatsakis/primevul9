  Simple_Selector_Obj Parser::parse_pseudo_selector() {
    if (lex< sequence<
          optional < pseudo_prefix >,
          // we keep the space within the name, strange enough
          // ToDo: refactor output to schedule the space for it
          // or do we really want to keep the real white-space?
          sequence< identifier, optional < block_comment >, exactly<'('> >
        > >())
    {

      std::string name(lexed);
      name.erase(name.size() - 1);
      ParserState p = pstate;

      // specially parse static stuff
      // ToDo: really everything static?
      if (peek_css <
            sequence <
              alternatives <
                static_value,
                binomial
              >,
              optional_css_whitespace,
              exactly<')'>
            >
          >()
      ) {
        lex_css< alternatives < static_value, binomial > >();
        String_Constant_Obj expr = SASS_MEMORY_NEW(String_Constant, pstate, lexed);
        if (lex_css< exactly<')'> >()) {
          expr->can_compress_whitespace(true);
          return SASS_MEMORY_NEW(Pseudo_Selector, p, name, expr);
        }
      }
      else if (Selector_List_Obj wrapped = parse_selector_list(true)) {
        if (wrapped && lex_css< exactly<')'> >()) {
          return SASS_MEMORY_NEW(Wrapped_Selector, p, name, wrapped);
        }
      }

    }
    // EO if pseudo selector

    else if (lex < sequence< optional < pseudo_prefix >, identifier > >()) {
      return SASS_MEMORY_NEW(Pseudo_Selector, pstate, lexed);
    }
    else if(lex < pseudo_prefix >()) {
      css_error("Invalid CSS", " after ", ": expected pseudoclass or pseudoelement, was ");
    }

    css_error("Invalid CSS", " after ", ": expected \")\", was ");

    // unreachable statement
    return {};
  }