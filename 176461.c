  Parameter_Obj Parser::parse_parameter()
  {
    if (peek< alternatives< exactly<','>, exactly< '{' >, exactly<';'> > >()) {
      css_error("Invalid CSS", " after ", ": expected variable (e.g. $foo), was ");
    }
    while (lex< alternatives < spaces, block_comment > >());
    lex < variable >();
    std::string name(Util::normalize_underscores(lexed));
    ParserState pos = pstate;
    Expression_Obj val;
    bool is_rest = false;
    while (lex< alternatives < spaces, block_comment > >());
    if (lex< exactly<':'> >()) { // there's a default value
      while (lex< block_comment >());
      val = parse_space_list();
    }
    else if (lex< exactly< ellipsis > >()) {
      is_rest = true;
    }
    return SASS_MEMORY_NEW(Parameter, pos, name, val, is_rest);
  }