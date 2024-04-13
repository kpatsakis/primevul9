  Content_Obj Parser::parse_content_directive()
  {
    return SASS_MEMORY_NEW(Content, pstate);
  }