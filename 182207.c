void CtcpParser::sendCtcpEvent(CtcpEvent *e) {
  CoreNetwork *net = coreNetwork(e);
  if(e->type() == EventManager::CtcpEvent) {
    QByteArray quotedReply;
    QString bufname = nickFromMask(e->prefix());
    if(e->ctcpType() == CtcpEvent::Query && !e->reply().isNull()) {
      if(_replies.contains(e->uuid()))
        _replies[e->uuid()].replies << lowLevelQuote(pack(net->serverEncode(e->ctcpCmd()),
                                                          net->userEncode(bufname, e->reply())));
      else
        // reply not caused by a request processed in here, so send it off immediately
        reply(net, bufname, e->ctcpCmd(), e->reply());
    }
  } else if(e->type() == EventManager::CtcpEventFlush && _replies.contains(e->uuid())) {
    CtcpReply reply = _replies.take(e->uuid());
    if(reply.replies.count())
      packedReply(net, reply.bufferName, reply.replies);
  }
}