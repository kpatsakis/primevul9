  Complex_Selector_Obj Parser::parse_complex_selector(bool chroot)
  {

    NESTING_GUARD(nestings);
    String_Obj reference;
    lex < block_comment >();
    advanceToNextToken();
    Complex_Selector_Obj sel = SASS_MEMORY_NEW(Complex_Selector, pstate);

    if (peek < end_of_file >()) return {};

    // parse the left hand side
    Compound_Selector_Obj lhs;
    // special case if it starts with combinator ([+~>])
    if (!peek_css< class_char < selector_combinator_ops > >()) {
      // parse the left hand side
      lhs = parse_compound_selector();
    }


    // parse combinator between lhs and rhs
    Complex_Selector::Combinator combinator = Complex_Selector::ANCESTOR_OF;
    if      (lex< exactly<'+'> >()) combinator = Complex_Selector::ADJACENT_TO;
    else if (lex< exactly<'~'> >()) combinator = Complex_Selector::PRECEDES;
    else if (lex< exactly<'>'> >()) combinator = Complex_Selector::PARENT_OF;
    else if (lex< sequence < exactly<'/'>, negate < exactly < '*' > > > >()) {
      // comments are allowed, but not spaces?
      combinator = Complex_Selector::REFERENCE;
      if (!lex < re_reference_combinator >()) return {};
      reference = SASS_MEMORY_NEW(String_Constant, pstate, lexed);
      if (!lex < exactly < '/' > >()) return {}; // ToDo: error msg?
    }

    if (!lhs && combinator == Complex_Selector::ANCESTOR_OF) return {};

    // lex < block_comment >();
    sel->head(lhs);
    sel->combinator(combinator);
    sel->media_block(last_media_block);

    if (combinator == Complex_Selector::REFERENCE) sel->reference(reference);
    // has linfeed after combinator?
    sel->has_line_break(peek_newline());
    // sel->has_line_feed(has_line_feed);

    // check if we got the abort condition (ToDo: optimize)
    if (!peek_css< class_char < complex_selector_delims > >()) {
      // parse next selector in sequence
      sel->tail(parse_complex_selector(true));
    }

    // add a parent selector if we are not in a root
    // also skip adding parent ref if we only have refs
    if (!sel->has_parent_ref() && !chroot) {
      // create the objects to wrap parent selector reference
      Compound_Selector_Obj head = SASS_MEMORY_NEW(Compound_Selector, pstate);
      Parent_Selector* parent = SASS_MEMORY_NEW(Parent_Selector, pstate, false);
      parent->media_block(last_media_block);
      head->media_block(last_media_block);
      // add simple selector
      head->append(parent);
      // selector may not have any head yet
      if (!sel->head()) { sel->head(head); }
      // otherwise we need to create a new complex selector and set the old one as its tail
      else {
        sel = SASS_MEMORY_NEW(Complex_Selector, pstate, Complex_Selector::ANCESTOR_OF, head, sel);
        sel->media_block(last_media_block);
      }
      // peek for linefeed and remember result on head
      // if (peek_newline()) head->has_line_break(true);
    }

    sel->update_pstate(pstate);
    // complex selector
    return sel;
  }