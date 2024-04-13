bool AbstractWebApplication::sessionEnd()
{
    if ((session_ != 0) && (sessions_.contains(session_->id))) {
        QNetworkCookie cookie(C_SID, session_->id.toUtf8());
        cookie.setPath(QLatin1String("/"));
        cookie.setExpirationDate(QDateTime::currentDateTime());

        sessions_.remove(session_->id);
        delete session_;
        session_ = 0;

        header(Http::HEADER_SET_COOKIE, cookie.toRawForm());
        return true;
    }

    return false;
}