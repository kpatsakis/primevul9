  Number* Parser::lexed_dimension(const ParserState& pstate, const std::string& parsed)
  {
    size_t L = parsed.length();
    size_t num_pos = parsed.find_first_not_of(" \n\r\t");
    if (num_pos == std::string::npos) num_pos = L;
    size_t unit_pos = parsed.find_first_not_of("-+0123456789.", num_pos);
    if (parsed[unit_pos] == 'e' && is_number(parsed[unit_pos+1]) ) {
      unit_pos = parsed.find_first_not_of("-+0123456789.", ++ unit_pos);
    }
    if (unit_pos == std::string::npos) unit_pos = L;
    const std::string& num = parsed.substr(num_pos, unit_pos - num_pos);
    Number* nr = SASS_MEMORY_NEW(Number,
                                    pstate,
                                    sass_strtod(num.c_str()),
                                    Token(number(parsed.c_str())),
                                    number_has_zero(parsed));
    nr->is_interpolant(false);
    nr->is_delayed(true);
    return nr;
  }