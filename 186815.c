void AbstractWebApplication::removeInactiveSessions()
{
    const uint now = QDateTime::currentDateTime().toTime_t();

    foreach (const QString &id, sessions_.keys()) {
        if ((now - sessions_[id]->timestamp) > INACTIVE_TIME)
            delete sessions_.take(id);
    }
}