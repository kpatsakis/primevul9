  Number* Parser::lexed_percentage(const ParserState& pstate, const std::string& parsed)
  {
    Number* nr = SASS_MEMORY_NEW(Number,
                                    pstate,
                                    sass_strtod(parsed.c_str()),
                                    "%",
                                    true);
    nr->is_interpolant(false);
    nr->is_delayed(true);
    return nr;
  }