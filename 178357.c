  Number_Ptr Parser::lexed_number(const ParserState& pstate, const std::string& parsed)
  {
    Number_Ptr nr = SASS_MEMORY_NEW(Number,
                                    pstate,
                                    sass_strtod(parsed.c_str()),
                                    "",
                                    number_has_zero(parsed));
    nr->is_interpolant(false);
    nr->is_delayed(true);
    return nr;
  }