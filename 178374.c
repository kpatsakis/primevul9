  Argument_Obj Parser::parse_argument()
  {
    if (peek< alternatives< exactly<','>, exactly< '{' >, exactly<';'> > >()) {
      css_error("Invalid CSS", " after ", ": expected \")\", was ");
    }
    if (peek_css< sequence < exactly< hash_lbrace >, exactly< rbrace > > >()) {
      position += 2;
      css_error("Invalid CSS", " after ", ": expected expression (e.g. 1px, bold), was ");
    }

    Argument_Obj arg;
    if (peek_css< sequence < variable, optional_css_comments, exactly<':'> > >()) {
      lex_css< variable >();
      std::string name(Util::normalize_underscores(lexed));
      ParserState p = pstate;
      lex_css< exactly<':'> >();
      Expression_Obj val = parse_space_list();
      arg = SASS_MEMORY_NEW(Argument, p, val, name);
    }
    else {
      bool is_arglist = false;
      bool is_keyword = false;
      Expression_Obj val = parse_space_list();
      List_Ptr l = Cast<List>(val);
      if (lex_css< exactly< ellipsis > >()) {
        if (val->concrete_type() == Expression::MAP || (
           (l != NULL && l->separator() == SASS_HASH)
        )) is_keyword = true;
        else is_arglist = true;
      }
      arg = SASS_MEMORY_NEW(Argument, pstate, val, "", is_arglist, is_keyword);
    }
    return arg;
  }