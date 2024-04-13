  Directive_Obj Parser::parse_special_directive()
  {
    std::string kwd(lexed);

    if (lexed == "@else") error("Invalid CSS: @else must come after @if");

    // this whole branch is never hit via spec tests

    Directive* at_rule = SASS_MEMORY_NEW(Directive, pstate, kwd);
    Lookahead lookahead = lookahead_for_include(position);
    if (lookahead.found && !lookahead.has_interpolants) {
      at_rule->selector(parse_selector_list(false));
    }

    lex < css_comments >(false);

    if (lex < static_property >()) {
      at_rule->value(parse_interpolated_chunk(Token(lexed)));
    } else if (!(peek < alternatives < exactly<'{'>, exactly<'}'>, exactly<';'> > >())) {
      at_rule->value(parse_list());
    }

    lex < css_comments >(false);

    if (peek< exactly<'{'> >()) {
      at_rule->block(parse_block());
    }

    return at_rule;
  }