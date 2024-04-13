void CtcpParser::displayMsg(NetworkEvent *event, Message::Type msgType, const QString &msg, const QString &sender,
                            const QString &target, Message::Flags msgFlags) {
  if(event->testFlag(EventManager::Silent))
    return;

  MessageEvent *msgEvent = new MessageEvent(msgType, event->network(), msg, sender, target, msgFlags);
  msgEvent->setTimestamp(event->timestamp());

  emit newEvent(msgEvent);
}