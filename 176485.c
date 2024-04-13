  List_Obj Parser::parse_media_queries()
  {
    advanceToNextToken();
    List_Obj queries = SASS_MEMORY_NEW(List, pstate, 0, SASS_COMMA);
    if (!peek_css < exactly <'{'> >()) queries->append(parse_media_query());
    while (lex_css < exactly <','> >()) queries->append(parse_media_query());
    queries->update_pstate(pstate);
    return queries.detach();
  }