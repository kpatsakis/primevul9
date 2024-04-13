void CtcpParser::reply(CoreNetwork *net, const QString &bufname, const QString &ctcpTag, const QString &message) {
  QList<QByteArray> params;
  params << net->serverEncode(bufname) << lowLevelQuote(pack(net->serverEncode(ctcpTag), net->userEncode(bufname, message)));
  net->putCmd("NOTICE", params);
}