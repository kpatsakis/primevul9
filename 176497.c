  Function_Call_Obj Parser::parse_function_call()
  {
    lex< identifier >();
    std::string name(lexed);

    if (Util::normalize_underscores(name) == "content-exists" && stack.back() != Scope::Mixin)
    { error("Cannot call content-exists() except within a mixin."); }

    ParserState call_pos = pstate;
    Arguments_Obj args = parse_arguments();
    return SASS_MEMORY_NEW(Function_Call, call_pos, name, args);
  }