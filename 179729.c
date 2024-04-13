  void Parser::error(std::string msg, Position pos)
  {
    Position p(pos.line ? pos : before_token);
    ParserState pstate(path, source, p, Offset(0, 0));
    traces.push_back(Backtrace(pstate));
    throw Exception::InvalidSass(pstate, traces, msg);
  }