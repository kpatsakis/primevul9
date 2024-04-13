  Declaration_Obj Parser::parse_declaration() {
    String_Obj prop;
    bool is_custom_property = false;
    if (lex< sequence< optional< exactly<'*'> >, identifier_schema > >()) {
      const std::string property(lexed);
      is_custom_property = property.compare(0, 2, "--") == 0;
      prop = parse_identifier_schema();
    }
    else if (lex< sequence< optional< exactly<'*'> >, identifier, zero_plus< block_comment > > >()) {
      const std::string property(lexed);
      is_custom_property = property.compare(0, 2, "--") == 0;
      prop = SASS_MEMORY_NEW(String_Constant, pstate, lexed);
    }
    else {
      css_error("Invalid CSS", " after ", ": expected \"}\", was ");
    }
    bool is_indented = true;
    const std::string property(lexed);
    if (!lex_css< one_plus< exactly<':'> > >()) error("property \"" + escape_string(property)  + "\" must be followed by a ':'");
    if (!is_custom_property && match< sequence< optional_css_comments, exactly<';'> > >()) error("style declaration must contain a value");
    if (match< sequence< optional_css_comments, exactly<'{'> > >()) is_indented = false; // don't indent if value is empty
    if (is_custom_property) {
      return SASS_MEMORY_NEW(Declaration, prop->pstate(), prop, parse_css_variable_value(), false, true);
    }
    lex < css_comments >(false);
    if (peek_css< static_value >()) {
      return SASS_MEMORY_NEW(Declaration, prop->pstate(), prop, parse_static_value()/*, lex<kwd_important>()*/);
    }
    else {
      Expression_Obj value;
      Lookahead lookahead = lookahead_for_value(position);
      if (lookahead.found) {
        if (lookahead.has_interpolants) {
          value = parse_value_schema(lookahead.found);
        } else {
          value = parse_list(DELAYED);
        }
      }
      else {
        value = parse_list(DELAYED);
        if (List* list = Cast<List>(value)) {
          if (!list->is_bracketed() && list->length() == 0 && !peek< exactly <'{'> >()) {
            css_error("Invalid CSS", " after ", ": expected expression (e.g. 1px, bold), was ");
          }
        }
      }
      lex < css_comments >(false);
      Declaration_Obj decl = SASS_MEMORY_NEW(Declaration, prop->pstate(), prop, value/*, lex<kwd_important>()*/);
      decl->is_indented(is_indented);
      decl->update_pstate(pstate);
      return decl;
    }
  }