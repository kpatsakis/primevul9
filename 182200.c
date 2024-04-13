void CtcpParser::processIrcEventRawPrivmsg(IrcEventRawMessage *event) {
  parse(event, Message::Plain);
}