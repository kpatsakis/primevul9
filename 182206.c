QByteArray CtcpParser::pack(const QByteArray &ctcpTag, const QByteArray &message) {
  if(message.isEmpty())
    return XDELIM + ctcpTag + XDELIM;

  return XDELIM + ctcpTag + ' ' + xdelimQuote(message) + XDELIM;
}