  Mixin_Call_Obj Parser::parse_include_directive()
  {
    // lex identifier into `lexed` var
    lex_identifier(); // may error out
    // normalize underscores to hyphens
    std::string name(Util::normalize_underscores(lexed));
    // create the initial mixin call object
    Mixin_Call_Obj call = SASS_MEMORY_NEW(Mixin_Call, pstate, name, {}, {}, {});
    // parse mandatory arguments
    call->arguments(parse_arguments());
    // parse using and optional block parameters
    bool has_parameters = lex< kwd_using >() != nullptr;

    if (has_parameters) {
      if (!peek< exactly<'('> >()) css_error("Invalid CSS", " after ", ": expected \"(\", was ");
    } else {
      if (peek< exactly<'('> >()) css_error("Invalid CSS", " after ", ": expected \";\", was ");
    }

    if (has_parameters) call->block_parameters(parse_parameters());

    // parse optional block
    if (peek < exactly <'{'> >()) {
      call->block(parse_block());
    }
    else if (has_parameters)  {
      css_error("Invalid CSS", " after ", ": expected \"{\", was ");
    }
    // return ast node
    return call.detach();
  }