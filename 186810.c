void AbstractWebApplication::UnbanTimerEvent()
{
    UnbanTimer* ubantimer = static_cast<UnbanTimer*>(sender());
    qDebug("Ban period has expired for %s", qPrintable(ubantimer->peerIp().toString()));
    clientFailedAttempts_.remove(ubantimer->peerIp());
    ubantimer->deleteLater();
}