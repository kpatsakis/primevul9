  Definition_Obj Parser::parse_definition(Definition::Type which_type)
  {
    std::string which_str(lexed);
    if (!lex< identifier >()) error("invalid name in " + which_str + " definition");
    std::string name(Util::normalize_underscores(lexed));
    if (which_type == Definition::FUNCTION && (name == "and" || name == "or" || name == "not"))
    { error("Invalid function name \"" + name + "\"."); }
    ParserState source_position_of_def = pstate;
    Parameters_Obj params = parse_parameters();
    if (which_type == Definition::MIXIN) stack.push_back(Scope::Mixin);
    else stack.push_back(Scope::Function);
    Block_Obj body = parse_block();
    stack.pop_back();
    return SASS_MEMORY_NEW(Definition, source_position_of_def, name, params, body, which_type);
  }