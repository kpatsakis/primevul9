  Selector_Schema_Obj Parser::parse_selector_schema(const char* end_of_selector, bool chroot)
  {
    NESTING_GUARD(nestings);
    // move up to the start
    lex< optional_spaces >();
    const char* i = position;
    // selector schema re-uses string schema implementation
    String_Schema_Ptr schema = SASS_MEMORY_NEW(String_Schema, pstate);
    // the selector schema is pretty much just a wrapper for the string schema
    Selector_Schema_Obj selector_schema = SASS_MEMORY_NEW(Selector_Schema, pstate, schema);
    selector_schema->connect_parent(chroot == false);
    selector_schema->media_block(last_media_block);

    // process until end
    while (i < end_of_selector) {
      // try to parse mutliple interpolants
      if (const char* p = find_first_in_interval< exactly<hash_lbrace>, block_comment >(i, end_of_selector)) {
        // accumulate the preceding segment if the position has advanced
        if (i < p) {
          std::string parsed(i, p);
          String_Constant_Obj str = SASS_MEMORY_NEW(String_Constant, pstate, parsed);
          pstate += Offset(parsed);
          str->update_pstate(pstate);
          schema->append(str);
        }

        // skip over all nested inner interpolations up to our own delimiter
        const char* j = skip_over_scopes< exactly<hash_lbrace>, exactly<rbrace> >(p + 2, end_of_selector);
        // check if the interpolation never ends of only contains white-space (error out)
        if (!j || peek < sequence < optional_spaces, exactly<rbrace> > >(p+2)) {
          position = p+2;
          css_error("Invalid CSS", " after ", ": expected expression (e.g. 1px, bold), was ");
        }
        // pass inner expression to the parser to resolve nested interpolations
        pstate.add(p, p+2);
        Expression_Obj interpolant = Parser::from_c_str(p+2, j, ctx, traces, pstate).parse_list();
        // set status on the list expression
        interpolant->is_interpolant(true);
        // schema->has_interpolants(true);
        // add to the string schema
        schema->append(interpolant);
        // advance parser state
        pstate.add(p+2, j);
        // advance position
        i = j;
      }
      // no more interpolants have been found
      // add the last segment if there is one
      else {
        // make sure to add the last bits of the string up to the end (if any)
        if (i < end_of_selector) {
          std::string parsed(i, end_of_selector);
          String_Constant_Obj str = SASS_MEMORY_NEW(String_Constant, pstate, parsed);
          pstate += Offset(parsed);
          str->update_pstate(pstate);
          i = end_of_selector;
          schema->append(str);
        }
        // exit loop
      }
    }
    // EO until eos

    // update position
    position = i;

    // update for end position
    selector_schema->update_pstate(pstate);
    schema->update_pstate(pstate);

    after_token = before_token = pstate;

    // return parsed result
    return selector_schema.detach();
  }