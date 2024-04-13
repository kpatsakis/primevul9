  String_Schema_Obj Parser::parse_css_variable_value()
  {
    String_Schema_Obj schema = SASS_MEMORY_NEW(String_Schema, pstate);
    std::vector<char> brackets;
    while (true) {
      if (
        (brackets.empty() && lex< css_variable_top_level_value >(false)) ||
        (!brackets.empty() && lex< css_variable_value >(false))
      ) {
        Token str(lexed);
        schema->append(SASS_MEMORY_NEW(String_Constant, pstate, str));
      } else if (Expression_Obj tok = lex_interpolation()) {
        if (String_Schema* s = Cast<String_Schema>(tok)) {
          if (s->empty()) break;
          schema->concat(s);
        } else {
          schema->append(tok);
        }
      } else if (lex< quoted_string >()) {
        Expression_Obj tok = parse_string();
        if (tok.isNull()) break;
        if (String_Schema* s = Cast<String_Schema>(tok)) {
          if (s->empty()) break;
          schema->concat(s);
        } else {
          schema->append(tok);
        }
      } else if (lex< alternatives< exactly<'('>, exactly<'['>, exactly<'{'> > >()) {
        const char opening_bracket = *(position - 1);
        brackets.push_back(opening_bracket);
        schema->append(SASS_MEMORY_NEW(String_Constant, pstate, std::string(1, opening_bracket)));
      } else if (const char *match = peek< alternatives< exactly<')'>, exactly<']'>, exactly<'}'> > >()) {
        if (brackets.empty()) break;
        const char closing_bracket = *(match - 1);
        if (brackets.back() != Util::opening_bracket_for(closing_bracket)) {
          std::string message = ": expected \"";
          message += Util::closing_bracket_for(brackets.back());
          message += "\", was ";
          css_error("Invalid CSS", " after ", message);
        }
        lex< alternatives< exactly<')'>, exactly<']'>, exactly<'}'> > >();
        schema->append(SASS_MEMORY_NEW(String_Constant, pstate, std::string(1, closing_bracket)));
        brackets.pop_back();
      } else {
        break;
      }
    }

    if (!brackets.empty()) {
      std::string message = ": expected \"";
      message += Util::closing_bracket_for(brackets.back());
      message += "\", was ";
      css_error("Invalid CSS", " after ", message);
    }

    if (schema->empty()) return {};
    return schema.detach();
  }