  Function_Call_Obj Parser::parse_calc_function()
  {
    lex< identifier >();
    std::string name(lexed);
    ParserState call_pos = pstate;
    lex< exactly<'('> >();
    ParserState arg_pos = pstate;
    const char* arg_beg = position;
    parse_list();
    const char* arg_end = position;
    lex< skip_over_scopes <
          exactly < '(' >,
          exactly < ')' >
        > >();

    Argument_Obj arg = SASS_MEMORY_NEW(Argument, arg_pos, parse_interpolated_chunk(Token(arg_beg, arg_end)));
    Arguments_Obj args = SASS_MEMORY_NEW(Arguments, arg_pos);
    args->append(arg);
    return SASS_MEMORY_NEW(Function_Call, call_pos, name, args);
  }