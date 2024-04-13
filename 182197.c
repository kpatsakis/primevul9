QByteArray CtcpParser::lowLevelQuote(const QByteArray &message) {
  QByteArray quotedMessage = message;

  QHash<QByteArray, QByteArray> quoteHash = _ctcpMDequoteHash;
  QByteArray MQUOTE = QByteArray("\020");
  quoteHash.remove(MQUOTE + MQUOTE);
  quotedMessage.replace(MQUOTE, MQUOTE + MQUOTE);

  QHash<QByteArray, QByteArray>::const_iterator quoteIter = quoteHash.constBegin();
  while(quoteIter != quoteHash.constEnd()) {
    quotedMessage.replace(quoteIter.value(), quoteIter.key());
    quoteIter++;
  }
  return quotedMessage;
}