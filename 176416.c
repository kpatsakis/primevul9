  Assignment_Obj Parser::parse_assignment()
  {
    std::string name(Util::normalize_underscores(lexed));
    ParserState var_source_position = pstate;
    if (!lex< exactly<':'> >()) error("expected ':' after " + name + " in assignment statement");
    if (peek_css< alternatives < exactly<';'>, end_of_file > >()) {
      css_error("Invalid CSS", " after ", ": expected expression (e.g. 1px, bold), was ");
    }
    Expression_Obj val;
    Lookahead lookahead = lookahead_for_value(position);
    if (lookahead.has_interpolants && lookahead.found) {
      val = parse_value_schema(lookahead.found);
    } else {
      val = parse_list();
    }
    bool is_default = false;
    bool is_global = false;
    while (peek< alternatives < default_flag, global_flag > >()) {
      if (lex< default_flag >()) is_default = true;
      else if (lex< global_flag >()) is_global = true;
    }
    return SASS_MEMORY_NEW(Assignment, var_source_position, name, val, is_default, is_global);
  }