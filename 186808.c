bool AbstractWebApplication::sessionStart()
{
    if (session_ == 0) {
        session_ = new WebSession(generateSid());
        sessions_[session_->id] = session_;

        QNetworkCookie cookie(C_SID, session_->id.toUtf8());
        cookie.setPath(QLatin1String("/"));
        header(Http::HEADER_SET_COOKIE, cookie.toRawForm());

        return true;
    }

    return false;
}