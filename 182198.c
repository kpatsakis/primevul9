void CtcpParser::processIrcEventRawNotice(IrcEventRawMessage *event) {
  parse(event, Message::Notice);
}