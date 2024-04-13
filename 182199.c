QByteArray CtcpParser::xdelimDequote(const QByteArray &message) {
  QByteArray dequotedMessage;
  QByteArray messagepart;
  QHash<QByteArray, QByteArray>::iterator xdelimquote;

  for(int i = 0; i < message.size(); i++) {
    messagepart = message.mid(i,1);
    if(i+1 < message.size()) {
      for(xdelimquote = _ctcpXDelimDequoteHash.begin(); xdelimquote != _ctcpXDelimDequoteHash.end(); ++xdelimquote) {
        if(message.mid(i,2) == xdelimquote.key()) {
          messagepart = xdelimquote.value();
          i++;
          break;
        }
      }
    }
    dequotedMessage += messagepart;
  }
  return dequotedMessage;
}