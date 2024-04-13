  Content_Obj Parser::parse_content_directive()
  {
    ParserState call_pos = pstate;
    Arguments_Obj args = parse_arguments();

    return SASS_MEMORY_NEW(Content, call_pos, args);
  }