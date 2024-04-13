  Mixin_Call_Obj Parser::parse_include_directive()
  {
    // lex identifier into `lexed` var
    lex_identifier(); // may error out
    // normalize underscores to hyphens
    std::string name(Util::normalize_underscores(lexed));
    // create the initial mixin call object
    Mixin_Call_Obj call = SASS_MEMORY_NEW(Mixin_Call, pstate, name, {}, {});
    // parse mandatory arguments
    call->arguments(parse_arguments());
    // parse optional block
    if (peek < exactly <'{'> >()) {
      call->block(parse_block());
    }
    // return ast node
    return call.detach();
  }