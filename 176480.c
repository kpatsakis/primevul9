  Parameters_Obj Parser::parse_parameters()
  {
    Parameters_Obj params = SASS_MEMORY_NEW(Parameters, pstate);
    if (lex_css< exactly<'('> >()) {
      // if there's anything there at all
      if (!peek_css< exactly<')'> >()) {
        do {
          if (peek< exactly<')'> >()) break;
          params->append(parse_parameter());
        } while (lex_css< exactly<','> >());
      }
      if (!lex_css< exactly<')'> >()) {
        css_error("Invalid CSS", " after ", ": expected \")\", was ");
      }
    }
    return params;
  }