QByteArray CtcpParser::lowLevelDequote(const QByteArray &message) {
  QByteArray dequotedMessage;
  QByteArray messagepart;
  QHash<QByteArray, QByteArray>::iterator ctcpquote;

  // copy dequote Message
  for(int i = 0; i < message.size(); i++) {
    messagepart = message.mid(i,1);
    if(i+1 < message.size()) {
      for(ctcpquote = _ctcpMDequoteHash.begin(); ctcpquote != _ctcpMDequoteHash.end(); ++ctcpquote) {
        if(message.mid(i,2) == ctcpquote.key()) {
          messagepart = ctcpquote.value();
          ++i;
          break;
        }
      }
    }
    dequotedMessage += messagepart;
  }
  return dequotedMessage;
}