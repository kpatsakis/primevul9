  bool Parser::parse_block_node(bool is_root) {

    Block_Obj block = block_stack.back();

    parse_block_comments();

    // throw away white-space
    // includes line comments
    lex < css_whitespace >();

    Lookahead lookahead_result;

    // also parse block comments

    // first parse everything that is allowed in functions
    if (lex < variable >(true)) { block->append(parse_assignment()); }
    else if (lex < kwd_err >(true)) { block->append(parse_error()); }
    else if (lex < kwd_dbg >(true)) { block->append(parse_debug()); }
    else if (lex < kwd_warn >(true)) { block->append(parse_warning()); }
    else if (lex < kwd_if_directive >(true)) { block->append(parse_if_directive()); }
    else if (lex < kwd_for_directive >(true)) { block->append(parse_for_directive()); }
    else if (lex < kwd_each_directive >(true)) { block->append(parse_each_directive()); }
    else if (lex < kwd_while_directive >(true)) { block->append(parse_while_directive()); }
    else if (lex < kwd_return_directive >(true)) { block->append(parse_return_directive()); }

    // parse imports to process later
    else if (lex < kwd_import >(true)) {
      Scope parent = stack.empty() ? Scope::Rules : stack.back();
      if (parent != Scope::Function && parent != Scope::Root && parent != Scope::Rules && parent != Scope::Media) {
        if (! peek_css< uri_prefix >(position)) { // this seems to go in ruby sass 3.4.20
          error("Import directives may not be used within control directives or mixins.");
        }
      }
      // this puts the parsed doc into sheets
      // import stub will fetch this in expand
      Import_Obj imp = parse_import();
      // if it is a url, we only add the statement
      if (!imp->urls().empty()) block->append(imp);
      // process all resources now (add Import_Stub nodes)
      for (size_t i = 0, S = imp->incs().size(); i < S; ++i) {
        block->append(SASS_MEMORY_NEW(Import_Stub, pstate, imp->incs()[i]));
      }
    }

    else if (lex < kwd_extend >(true)) {
      Lookahead lookahead = lookahead_for_include(position);
      if (!lookahead.found) css_error("Invalid CSS", " after ", ": expected selector, was ");
      Selector_List_Obj target;
      if (!lookahead.has_interpolants) {
        target = parse_selector_list(true);
      }
      else {
        target = SASS_MEMORY_NEW(Selector_List, pstate);
        target->schema(parse_selector_schema(lookahead.found, true));
      }

      block->append(SASS_MEMORY_NEW(Extension, pstate, target));
    }

    // selector may contain interpolations which need delayed evaluation
    else if (
      !(lookahead_result = lookahead_for_selector(position)).error &&
      !lookahead_result.is_custom_property
    )
    {
      block->append(parse_ruleset(lookahead_result));
    }

    // parse multiple specific keyword directives
    else if (lex < kwd_media >(true)) { block->append(parse_media_block()); }
    else if (lex < kwd_at_root >(true)) { block->append(parse_at_root_block()); }
    else if (lex < kwd_include_directive >(true)) { block->append(parse_include_directive()); }
    else if (lex < kwd_content_directive >(true)) { block->append(parse_content_directive()); }
    else if (lex < kwd_supports_directive >(true)) { block->append(parse_supports_directive()); }
    else if (lex < kwd_mixin >(true)) { block->append(parse_definition(Definition::MIXIN)); }
    else if (lex < kwd_function >(true)) { block->append(parse_definition(Definition::FUNCTION)); }

    // ignore the @charset directive for now
    else if (lex< kwd_charset_directive >(true)) { parse_charset_directive(); }

    // generic at keyword (keep last)
    else if (lex< re_special_directive >(true)) { block->append(parse_special_directive()); }
    else if (lex< re_prefixed_directive >(true)) { block->append(parse_prefixed_directive()); }
    else if (lex< at_keyword >(true)) { block->append(parse_directive()); }

    else if (is_root && stack.back() != Scope::AtRoot /* && block->is_root() */) {
      lex< css_whitespace >();
      if (position >= end) return true;
      css_error("Invalid CSS", " after ", ": expected 1 selector or at-rule, was ");
    }
    // parse a declaration
    else
    {
      // ToDo: how does it handle parse errors?
      // maybe we are expected to parse something?
      Declaration_Obj decl = parse_declaration();
      decl->tabs(indentation);
      block->append(decl);
      // maybe we have a "sub-block"
      if (peek< exactly<'{'> >()) {
        if (decl->is_indented()) ++ indentation;
        // parse a propset that rides on the declaration's property
        stack.push_back(Scope::Properties);
        decl->block(parse_block());
        stack.pop_back();
        if (decl->is_indented()) -- indentation;
      }
    }
    // something matched
    return true;
  }