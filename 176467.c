  Arguments_Obj Parser::parse_arguments()
  {
    Arguments_Obj args = SASS_MEMORY_NEW(Arguments, pstate);
    if (lex_css< exactly<'('> >()) {
      // if there's anything there at all
      if (!peek_css< exactly<')'> >()) {
        do {
          if (peek< exactly<')'> >()) break;
          args->append(parse_argument());
        } while (lex_css< exactly<','> >());
      }
      if (!lex_css< exactly<')'> >()) {
        css_error("Invalid CSS", " after ", ": expected expression (e.g. 1px, bold), was ");
      }
    }
    return args;
  }