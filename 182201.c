void CtcpParser::parse(IrcEventRawMessage *e, Message::Type messagetype) {
  QByteArray ctcp;

  //lowlevel message dequote
  QByteArray dequotedMessage = lowLevelDequote(e->rawMessage());

  CtcpEvent::CtcpType ctcptype = e->type() == EventManager::IrcEventRawNotice
      ? CtcpEvent::Reply
      : CtcpEvent::Query;

  Message::Flags flags = (ctcptype == CtcpEvent::Reply && !e->network()->isChannelName(e->target()))
                          ? Message::Redirected
                          : Message::None;

  QList<CtcpEvent *> ctcpEvents;
  QUuid uuid; // needed to group all replies together

  // extract tagged / extended data
  int xdelimPos = -1;
  int xdelimEndPos = -1;
  int spacePos = -1;
  while((xdelimPos = dequotedMessage.indexOf(XDELIM)) != -1) {
    if(xdelimPos > 0)
      displayMsg(e, messagetype, targetDecode(e, dequotedMessage.left(xdelimPos)), e->prefix(), e->target(), flags);

    xdelimEndPos = dequotedMessage.indexOf(XDELIM, xdelimPos + 1);
    if(xdelimEndPos == -1) {
      // no matching end delimiter found... treat rest of the message as ctcp
      xdelimEndPos = dequotedMessage.count();
    }
    ctcp = xdelimDequote(dequotedMessage.mid(xdelimPos + 1, xdelimEndPos - xdelimPos - 1));
    dequotedMessage = dequotedMessage.mid(xdelimEndPos + 1);

    //dispatch the ctcp command
    QString ctcpcmd = targetDecode(e, ctcp.left(spacePos));
    QString ctcpparam = targetDecode(e, ctcp.mid(spacePos + 1));

    spacePos = ctcp.indexOf(' ');
    if(spacePos != -1) {
      ctcpcmd = targetDecode(e, ctcp.left(spacePos));
      ctcpparam = targetDecode(e, ctcp.mid(spacePos + 1));
    } else {
      ctcpcmd = targetDecode(e, ctcp);
      ctcpparam = QString();
    }

    ctcpcmd = ctcpcmd.toUpper();

    // we don't want to block /me messages by the CTCP ignore list
    if(ctcpcmd == QLatin1String("ACTION") || !coreSession()->ignoreListManager()->ctcpMatch(e->prefix(), e->network()->networkName(), ctcpcmd)) {
      if(uuid.isNull())
        uuid = QUuid::createUuid();

      CtcpEvent *event = new CtcpEvent(EventManager::CtcpEvent, e->network(), e->prefix(), e->target(),
                                       ctcptype, ctcpcmd, ctcpparam, e->timestamp(), uuid);
      ctcpEvents << event;
    }
  }
  if(!ctcpEvents.isEmpty()) {
    _replies.insert(uuid, CtcpReply(coreNetwork(e), nickFromMask(e->prefix())));
    CtcpEvent *flushEvent = new CtcpEvent(EventManager::CtcpEventFlush, e->network(), e->prefix(), e->target(),
                                          ctcptype, "INVALID", QString(), e->timestamp(), uuid);
    ctcpEvents << flushEvent;
    foreach(CtcpEvent *event, ctcpEvents) {
      emit newEvent(event);
    }
  }

  if(!dequotedMessage.isEmpty())
    displayMsg(e, messagetype, targetDecode(e, dequotedMessage), e->prefix(), e->target(), flags);
}