  String_Obj Parser::parse_identifier_schema()
  {
    Token id(lexed);
    const char* i = id.begin;
    // see if there any interpolants
    const char* p = find_first_in_interval< exactly<hash_lbrace>, block_comment >(id.begin, id.end);
    if (!p) {
      return SASS_MEMORY_NEW(String_Constant, pstate, std::string(id.begin, id.end));
    }

    String_Schema_Obj schema = SASS_MEMORY_NEW(String_Schema, pstate);
    while (i < id.end) {
      p = find_first_in_interval< exactly<hash_lbrace>, block_comment >(i, id.end);
      if (p) {
        if (i < p) {
          // accumulate the preceding segment if it's nonempty
          const char* o = position; position = i;
          schema->append(parse_value_schema(p));
          position = o;
        }
        // we need to skip anything inside strings
        // create a new target in parser/prelexer
        if (peek < sequence < optional_spaces, exactly<rbrace> > >(p+2)) { position = p;
          css_error("Invalid CSS", " after ", ": expected expression (e.g. 1px, bold), was ");
        }
        const char* j = skip_over_scopes< exactly<hash_lbrace>, exactly<rbrace> >(p+2, id.end); // find the closing brace
        if (j) {
          // parse the interpolant and accumulate it
          Expression_Obj interp_node = Parser::from_token(Token(p+2, j), ctx, traces, pstate, source).parse_list(DELAYED);
          interp_node->is_interpolant(true);
          schema->append(interp_node);
          // schema->has_interpolants(true);
          i = j;
        }
        else {
          // throw an error if the interpolant is unterminated
          error("unterminated interpolant inside interpolated identifier " + id.to_string());
        }
      }
      else { // no interpolants left; add the last segment if nonempty
        if (i < end) {
          const char* o = position; position = i;
          schema->append(parse_value_schema(id.end));
          position = o;
        }
        break;
      }
    }
    return schema ? schema.detach() : 0;
  }