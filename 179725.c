  Import_Obj Parser::parse_import()
  {
    Import_Obj imp = SASS_MEMORY_NEW(Import, pstate);
    std::vector<std::pair<std::string,Function_Call_Obj>> to_import;
    bool first = true;
    do {
      while (lex< block_comment >());
      if (lex< quoted_string >()) {
        to_import.push_back(std::pair<std::string,Function_Call_Obj>(std::string(lexed), 0));
      }
      else if (lex< uri_prefix >()) {
        Arguments_Obj args = SASS_MEMORY_NEW(Arguments, pstate);
        Function_Call_Obj result = SASS_MEMORY_NEW(Function_Call, pstate, "url", args);

        if (lex< quoted_string >()) {
          Expression_Obj quoted_url = parse_string();
          args->append(SASS_MEMORY_NEW(Argument, quoted_url->pstate(), quoted_url));
        }
        else if (String_Obj string_url = parse_url_function_argument()) {
          args->append(SASS_MEMORY_NEW(Argument, string_url->pstate(), string_url));
        }
        else if (peek < skip_over_scopes < exactly < '(' >, exactly < ')' > > >(position)) {
          Expression_Obj braced_url = parse_list(); // parse_interpolated_chunk(lexed);
          args->append(SASS_MEMORY_NEW(Argument, braced_url->pstate(), braced_url));
        }
        else {
          error("malformed URL");
        }
        if (!lex< exactly<')'> >()) error("URI is missing ')'");
        to_import.push_back(std::pair<std::string, Function_Call_Obj>("", result));
      }
      else {
        if (first) error("@import directive requires a url or quoted path");
        else error("expecting another url or quoted path in @import list");
      }
      first = false;
    } while (lex_css< exactly<','> >());

    if (!peek_css< alternatives< exactly<';'>, exactly<'}'>, end_of_file > >()) {
      List_Obj import_queries = parse_media_queries();
      imp->import_queries(import_queries);
    }

    for(auto location : to_import) {
      if (location.second) {
        imp->urls().push_back(location.second);
      }
      // check if custom importers want to take over the handling
      else if (!ctx.call_importers(unquote(location.first), path, pstate, imp)) {
        // nobody wants it, so we do our import
        ctx.import_url(imp, location.first, path);
      }
    }

    return imp;
  }