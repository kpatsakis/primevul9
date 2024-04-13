  String_Schema_Obj Parser::parse_css_variable_value_token(bool top_level)
  {
    String_Schema_Obj schema = SASS_MEMORY_NEW(String_Schema, pstate);
    if (
      (top_level && lex< css_variable_top_level_value >(false)) ||
      (!top_level && lex< css_variable_value >(false))
    ) {
      Token str(lexed);
      schema->append(SASS_MEMORY_NEW(String_Constant, pstate, str));
    }
    else if (Expression_Obj tok = lex_interpolation()) {
      if (String_Schema_Ptr s = Cast<String_Schema>(tok)) {
        schema->concat(s);
      } else {
        schema->append(tok);
      }
    }
    else if (lex< quoted_string >()) {
      Expression_Obj tok = parse_string();
      if (String_Schema_Ptr s = Cast<String_Schema>(tok)) {
        schema->concat(s);
      } else {
        schema->append(tok);
      }
    }
    else {
      if (peek< alternatives< exactly<'('>, exactly<'['>, exactly<'{'> > >()) {
        if (lex< exactly<'('> >()) {
          schema->append(SASS_MEMORY_NEW(String_Constant, pstate, std::string("(")));
          if (String_Schema_Obj tok = parse_css_variable_value(false)) schema->concat(tok);
          if (!lex< exactly<')'> >()) css_error("Invalid CSS", " after ", ": expected \")\", was ");
          schema->append(SASS_MEMORY_NEW(String_Constant, pstate, std::string(")")));
        }
        else if (lex< exactly<'['> >()) {
          schema->append(SASS_MEMORY_NEW(String_Constant, pstate, std::string("[")));
          if (String_Schema_Obj tok = parse_css_variable_value(false)) schema->concat(tok);
          if (!lex< exactly<']'> >()) css_error("Invalid CSS", " after ", ": expected \"]\", was ");
          schema->append(SASS_MEMORY_NEW(String_Constant, pstate, std::string("]")));
        }
        else if (lex< exactly<'{'> >()) {
          schema->append(SASS_MEMORY_NEW(String_Constant, pstate, std::string("{")));
          if (String_Schema_Obj tok = parse_css_variable_value(false)) schema->concat(tok);
          if (!lex< exactly<'}'> >()) css_error("Invalid CSS", " after ", ": expected \"}\", was ");
          schema->append(SASS_MEMORY_NEW(String_Constant, pstate, std::string("}")));
        }
      }
    }

    return schema->length() > 0 ? schema.detach() : NULL;
  }