  Selector_List_Obj Parser::parse_selector_list(bool chroot)
  {
    bool reloop;
    bool had_linefeed = false;
    NESTING_GUARD(nestings);
    Complex_Selector_Obj sel;
    Selector_List_Obj group = SASS_MEMORY_NEW(Selector_List, pstate);
    group->media_block(last_media_block);

    if (peek_css< alternatives < end_of_file, exactly <'{'>, exactly <','> > >()) {
      css_error("Invalid CSS", " after ", ": expected selector, was ");
    }

    do {
      reloop = false;

      had_linefeed = had_linefeed || peek_newline();

      if (peek_css< alternatives < class_char < selector_list_delims > > >())
        break; // in case there are superfluous commas at the end

      // now parse the complex selector
      sel = parse_complex_selector(chroot);

      if (!sel) return group.detach();

      sel->has_line_feed(had_linefeed);

      had_linefeed = false;

      while (peek_css< exactly<','> >())
      {
        lex< css_comments >(false);
        // consume everything up and including the comma separator
        reloop = lex< exactly<','> >() != 0;
        // remember line break (also between some commas)
        had_linefeed = had_linefeed || peek_newline();
        // remember line break (also between some commas)
      }
      group->append(sel);
    }
    while (reloop);
    while (lex_css< kwd_optional >()) {
      group->is_optional(true);
    }
    // update for end position
    group->update_pstate(pstate);
    if (sel) sel->last()->has_line_break(false);
    return group.detach();
  }