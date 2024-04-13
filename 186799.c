    UnbanTimer(const QHostAddress& peer_ip, QObject *parent)
        : QTimer(parent), m_peerIp(peer_ip)
    {
        setSingleShot(true);
        setInterval(BAN_TIME);
    }