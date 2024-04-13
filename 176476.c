  void Parser::error(std::string msg, Position pos)
  {
    Position p(pos.line ? pos : before_token);
    ParserState pstate(path, source, p, Offset(0, 0));
    // `pstate.src` may not outlive stack unwind so we must copy it.
    char *src_copy = sass_copy_c_string(pstate.src);
    pstate.src = src_copy;
    traces.push_back(Backtrace(pstate));
    throw Exception::InvalidSass(pstate, traces, msg, src_copy);
  }