  Directive_Obj Parser::parse_directive()
  {
    Directive_Obj directive = SASS_MEMORY_NEW(Directive, pstate, lexed);
    String_Schema_Obj val = parse_almost_any_value();
    // strip left and right if they are of type string
    directive->value(val);
    if (peek< exactly<'{'> >()) {
      directive->block(parse_block());
    }
    return directive;
  }