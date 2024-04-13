bool AbstractWebApplication::sessionInitialize()
{
    static const QString SID_START = QLatin1String(C_SID) + QLatin1String("=");

    if (session_ == 0)
    {
        QString cookie = request_.headers.value("cookie");
        //qDebug() << Q_FUNC_INFO << "cookie: " << cookie;

        QString sessionId;
        int pos = cookie.indexOf(SID_START);
        if (pos >= 0) {
            pos += SID_START.length();
            int end = cookie.indexOf(QRegExp("[,;]"), pos);
            sessionId = cookie.mid(pos, end >= 0 ? end - pos : end);
        }

        // TODO: Additional session check

        if (!sessionId.isNull()) {
            if (sessions_.contains(sessionId)) {
                session_ = sessions_[sessionId];
                session_->updateTimestamp();
                return true;
            }
            else {
                qDebug() << Q_FUNC_INFO << "session does not exist!";
            }
        }
    }

    return false;
}