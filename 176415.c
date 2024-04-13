  String_Obj Parser::parse_ie_property()
  {
    lex< ie_property >();
    Token str(lexed);
    const char* i = str.begin;
    // see if there any interpolants
    const char* p = find_first_in_interval< exactly<hash_lbrace>, block_comment >(str.begin, str.end);
    if (!p) {
      return SASS_MEMORY_NEW(String_Quoted, pstate, std::string(str.begin, str.end));
    }

    String_Schema* schema = SASS_MEMORY_NEW(String_Schema, pstate);
    while (i < str.end) {
      p = find_first_in_interval< exactly<hash_lbrace>, block_comment >(i, str.end);
      if (p) {
        if (i < p) {
          schema->append(SASS_MEMORY_NEW(String_Constant, pstate, std::string(i, p))); // accumulate the preceding segment if it's nonempty
        }
        if (peek < sequence < optional_spaces, exactly<rbrace> > >(p+2)) { position = p+2;
          css_error("Invalid CSS", " after ", ": expected expression (e.g. 1px, bold), was ");
        }
        const char* j = skip_over_scopes< exactly<hash_lbrace>, exactly<rbrace> >(p+2, str.end); // find the closing brace
        if (j) {
          // parse the interpolant and accumulate it
          Expression_Obj interp_node = Parser::from_token(Token(p+2, j), ctx, traces, pstate, source).parse_list();
          interp_node->is_interpolant(true);
          schema->append(interp_node);
          i = j;
        }
        else {
          // throw an error if the interpolant is unterminated
          error("unterminated interpolant inside IE function " + str.to_string());
        }
      }
      else { // no interpolants left; add the last segment if nonempty
        if (i < str.end) {
          schema->append(SASS_MEMORY_NEW(String_Constant, pstate, std::string(i, str.end)));
        }
        break;
      }
    }
    return schema;
  }