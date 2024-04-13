CtcpParser::CtcpParser(CoreSession *coreSession, QObject *parent)
  : QObject(parent),
    _coreSession(coreSession)
{
  QByteArray MQUOTE = QByteArray("\020");
  _ctcpMDequoteHash[MQUOTE + '0'] = QByteArray(1, '\000');
  _ctcpMDequoteHash[MQUOTE + 'n'] = QByteArray(1, '\n');
  _ctcpMDequoteHash[MQUOTE + 'r'] = QByteArray(1, '\r');
  _ctcpMDequoteHash[MQUOTE + MQUOTE] = MQUOTE;

  QByteArray XQUOTE = QByteArray("\134");
  _ctcpXDelimDequoteHash[XQUOTE + XQUOTE] = XQUOTE;
  _ctcpXDelimDequoteHash[XQUOTE + QByteArray("a")] = XDELIM;

  connect(this, SIGNAL(newEvent(Event *)), _coreSession->eventManager(), SLOT(postEvent(Event *)));
}