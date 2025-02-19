  Compound_Selector_Obj Parser::parse_compound_selector()
  {
    // init an empty compound selector wrapper
    Compound_Selector_Obj seq = SASS_MEMORY_NEW(Compound_Selector, pstate);
    seq->media_block(last_media_block);

    // skip initial white-space
    lex< css_whitespace >();

    // parse list
    while (true)
    {
      // remove all block comments (don't skip white-space)
      lex< delimited_by< slash_star, star_slash, false > >(false);
      // parse functional
      if (match < re_pseudo_selector >())
      {
        seq->append(parse_simple_selector());
      }
      // parse parent selector
      else if (lex< exactly<'&'> >(false))
      {
        if (!allow_parent) error("Parent selectors aren't allowed here.");
        // this produces a linefeed!?
        seq->has_parent_reference(true);
        seq->append(SASS_MEMORY_NEW(Parent_Selector, pstate));
        // parent selector only allowed at start
        // upcoming Sass may allow also trailing
        if (seq->length() > 1) {
          ParserState state(pstate);
          Simple_Selector_Obj cur = (*seq)[seq->length()-1];
          Simple_Selector_Obj prev = (*seq)[seq->length()-2];
          std::string sel(prev->to_string({ NESTED, 5 }));
          std::string found(cur->to_string({ NESTED, 5 }));
          if (lex < identifier >()) { found += std::string(lexed); }
          error("Invalid CSS after \"" + sel + "\": expected \"{\", was \"" + found + "\"\n\n"
            "\"" + found + "\" may only be used at the beginning of a compound selector.", state);
        }
      }
      // parse type selector
      else if (lex< re_type_selector >(false))
      {
        seq->append(SASS_MEMORY_NEW(Type_Selector, pstate, lexed));
      }
      // peek for abort conditions
      else if (peek< spaces >()) break;
      else if (peek< end_of_file >()) { break; }
      else if (peek_css < class_char < selector_combinator_ops > >()) break;
      else if (peek_css < class_char < complex_selector_delims > >()) break;
      // otherwise parse another simple selector
      else {
        Simple_Selector_Obj sel = parse_simple_selector();
        if (!sel) return {};
        seq->append(sel);
      }
    }

    if (seq && !peek_css<alternatives<end_of_file,exactly<'{'>>>()) {
      seq->has_line_break(peek_newline());
    }

    // EO while true
    return seq;

  }