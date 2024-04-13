  Expression_Obj Parser::parse_factor()
  {
    NESTING_GUARD(nestings);
    lex < css_comments >(false);
    if (lex_css< exactly<'('> >()) {
      // parse_map may return a list
      Expression_Obj value = parse_map();
      // lex the expected closing parenthesis
      if (!lex_css< exactly<')'> >()) error("unclosed parenthesis");
      // expression can be evaluated
      return value;
    }
    else if (lex_css< exactly<'['> >()) {
      // explicit bracketed
      Expression_Obj value = parse_bracket_list();
      // lex the expected closing square bracket
      if (!lex_css< exactly<']'> >()) error("unclosed squared bracket");
      return value;
    }
    // string may be interpolated
    // if (lex< quoted_string >()) {
    //   return &parse_string();
    // }
    else if (peek< ie_property >()) {
      return parse_ie_property();
    }
    else if (peek< ie_keyword_arg >()) {
      return parse_ie_keyword_arg();
    }
    else if (peek< sequence < calc_fn_call, exactly <'('> > >()) {
      return parse_calc_function();
    }
    else if (lex < functional_schema >()) {
      return parse_function_call_schema();
    }
    else if (lex< identifier_schema >()) {
      String_Obj string = parse_identifier_schema();
      if (String_Schema* schema = Cast<String_Schema>(string)) {
        if (lex < exactly < '(' > >()) {
          schema->append(parse_list());
          lex < exactly < ')' > >();
        }
      }
      return string;
    }
    else if (peek< sequence< uri_prefix, W, real_uri_value > >()) {
      return parse_url_function_string();
    }
    else if (peek< re_functional >()) {
      return parse_function_call();
    }
    else if (lex< exactly<'+'> >()) {
      Unary_Expression* ex = SASS_MEMORY_NEW(Unary_Expression, pstate, Unary_Expression::PLUS, parse_factor());
      if (ex && ex->operand()) ex->is_delayed(ex->operand()->is_delayed());
      return ex;
    }
    else if (lex< exactly<'-'> >()) {
      Unary_Expression* ex = SASS_MEMORY_NEW(Unary_Expression, pstate, Unary_Expression::MINUS, parse_factor());
      if (ex && ex->operand()) ex->is_delayed(ex->operand()->is_delayed());
      return ex;
    }
    else if (lex< exactly<'/'> >()) {
      Unary_Expression* ex = SASS_MEMORY_NEW(Unary_Expression, pstate, Unary_Expression::SLASH, parse_factor());
      if (ex && ex->operand()) ex->is_delayed(ex->operand()->is_delayed());
      return ex;
    }
    else if (lex< sequence< kwd_not > >()) {
      Unary_Expression* ex = SASS_MEMORY_NEW(Unary_Expression, pstate, Unary_Expression::NOT, parse_factor());
      if (ex && ex->operand()) ex->is_delayed(ex->operand()->is_delayed());
      return ex;
    }
    // this whole branch is never hit via spec tests
    else if (peek < sequence < one_plus < alternatives < css_whitespace, exactly<'-'>, exactly<'+'> > >, number > >()) {
      if (parse_number_prefix()) return parse_value(); // prefix is positive
      Unary_Expression* ex = SASS_MEMORY_NEW(Unary_Expression, pstate, Unary_Expression::MINUS, parse_value());
      if (ex->operand()) ex->is_delayed(ex->operand()->is_delayed());
      return ex;
    }
    else {
      return parse_value();
    }
  }