QByteArray CtcpParser::xdelimQuote(const QByteArray &message) {
  QByteArray quotedMessage = message;
  QHash<QByteArray, QByteArray>::const_iterator quoteIter = _ctcpXDelimDequoteHash.constBegin();
  while(quoteIter != _ctcpXDelimDequoteHash.constEnd()) {
    quotedMessage.replace(quoteIter.value(), quoteIter.key());
    quoteIter++;
  }
  return quotedMessage;
}