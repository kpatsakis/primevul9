  String_Schema_Obj Parser::parse_value_schema(const char* stop)
  {
    // initialize the string schema object to add tokens
    String_Schema_Obj schema = SASS_MEMORY_NEW(String_Schema, pstate);

    if (peek<exactly<'}'>>()) {
      css_error("Invalid CSS", " after ", ": expected expression (e.g. 1px, bold), was ");
    }

    const char* e;
    const char* ee = end;
    end = stop;
    size_t num_items = 0;
    bool need_space = false;
    while (position < stop) {
      // parse space between tokens
      if (lex< spaces >() && num_items) {
        need_space = true;
      }
      if (need_space) {
        need_space = false;
        // schema->append(SASS_MEMORY_NEW(String_Constant, pstate, " "));
      }
      if ((e = peek< re_functional >()) && e < stop) {
        schema->append(parse_function_call());
      }
      // lex an interpolant /#{...}/
      else if (lex< exactly < hash_lbrace > >()) {
        // Try to lex static expression first
        if (peek< exactly< rbrace > >()) {
          css_error("Invalid CSS", " after ", ": expected expression (e.g. 1px, bold), was ");
        }
        Expression_Obj ex;
        if (lex< re_static_expression >()) {
          ex = SASS_MEMORY_NEW(String_Constant, pstate, lexed);
        } else {
          ex = parse_list(true);
        }
        ex->is_interpolant(true);
        schema->append(ex);
        if (!lex < exactly < rbrace > >()) {
          css_error("Invalid CSS", " after ", ": expected \"}\", was ");
        }
      }
      // lex some string constants or other valid token
      // Note: [-+] chars are left over from i.e. `#{3}+3`
      else if (lex< alternatives < exactly<'%'>, exactly < '-' >, exactly < '+' > > >()) {
        schema->append(SASS_MEMORY_NEW(String_Constant, pstate, lexed));
      }
      // lex a quoted string
      else if (lex< quoted_string >()) {
        // need_space = true;
        // if (schema->length()) schema->append(SASS_MEMORY_NEW(String_Constant, pstate, " "));
        // else need_space = true;
        schema->append(parse_string());
        if ((*position == '"' || *position == '\'') || peek < alternatives < alpha > >()) {
          // need_space = true;
        }
        if (peek < exactly < '-' > >()) break;
      }
      else if (lex< identifier >()) {
        schema->append(SASS_MEMORY_NEW(String_Constant, pstate, lexed));
        if ((*position == '"' || *position == '\'') || peek < alternatives < alpha > >()) {
           // need_space = true;
        }
      }
      // lex (normalized) variable
      else if (lex< variable >()) {
        std::string name(Util::normalize_underscores(lexed));
        schema->append(SASS_MEMORY_NEW(Variable, pstate, name));
      }
      // lex percentage value
      else if (lex< percentage >()) {
        schema->append(lexed_percentage(lexed));
      }
      // lex dimension value
      else if (lex< dimension >()) {
        schema->append(lexed_dimension(lexed));
      }
      // lex number value
      else if (lex< number >()) {
        schema->append(lexed_number(lexed));
      }
      // lex hex color value
      else if (lex< sequence < hex, negate < exactly < '-' > > > >()) {
        schema->append(lexed_hex_color(lexed));
      }
      else if (lex< sequence < exactly <'#'>, identifier > >()) {
        schema->append(SASS_MEMORY_NEW(String_Quoted, pstate, lexed));
      }
      // lex a value in parentheses
      else if (peek< parenthese_scope >()) {
        schema->append(parse_factor());
      }
      else {
        break;
      }
      ++num_items;
    }
    if (position != stop) {
      schema->append(SASS_MEMORY_NEW(String_Constant, pstate, std::string(position, stop)));
      position = stop;
    }
    end = ee;
    return schema;
  }