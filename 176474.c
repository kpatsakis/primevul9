  Media_Query_Obj Parser::parse_media_query()
  {
    advanceToNextToken();
    Media_Query_Obj media_query = SASS_MEMORY_NEW(Media_Query, pstate);
    if (lex < kwd_not >()) { media_query->is_negated(true); lex < css_comments >(false); }
    else if (lex < kwd_only >()) { media_query->is_restricted(true); lex < css_comments >(false); }

    if (lex < identifier_schema >())  media_query->media_type(parse_identifier_schema());
    else if (lex < identifier >())    media_query->media_type(parse_interpolated_chunk(lexed));
    else                             media_query->append(parse_media_expression());

    while (lex_css < kwd_and >()) media_query->append(parse_media_expression());
    if (lex < identifier_schema >()) {
      String_Schema* schema = SASS_MEMORY_NEW(String_Schema, pstate);
      schema->append(media_query->media_type());
      schema->append(SASS_MEMORY_NEW(String_Constant, pstate, " "));
      schema->append(parse_identifier_schema());
      media_query->media_type(schema);
    }
    while (lex_css < kwd_and >()) media_query->append(parse_media_expression());

    media_query->update_pstate(pstate);

    return media_query;
  }