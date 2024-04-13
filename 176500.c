  Simple_Selector_Obj Parser::parse_simple_selector()
  {
    lex < css_comments >(false);
    if (lex< class_name >()) {
      return SASS_MEMORY_NEW(Class_Selector, pstate, lexed);
    }
    else if (lex< id_name >()) {
      return SASS_MEMORY_NEW(Id_Selector, pstate, lexed);
    }
    else if (lex< alternatives < variable, number, static_reference_combinator > >()) {
      return SASS_MEMORY_NEW(Type_Selector, pstate, lexed);
    }
    else if (peek< pseudo_not >()) {
      return parse_negated_selector();
    }
    else if (peek< re_pseudo_selector >()) {
      return parse_pseudo_selector();
    }
    else if (peek< exactly<':'> >()) {
      return parse_pseudo_selector();
    }
    else if (lex < exactly<'['> >()) {
      return parse_attribute_selector();
    }
    else if (lex< placeholder >()) {
      Placeholder_Selector* sel = SASS_MEMORY_NEW(Placeholder_Selector, pstate, lexed);
      sel->media_block(last_media_block);
      return sel;
    }
    else {
      css_error("Invalid CSS", " after ", ": expected selector, was ");
    }
    // failed
    return {};
  }