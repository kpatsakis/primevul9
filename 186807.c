WebSessionData *AbstractWebApplication::session()
{
    Q_ASSERT(session_ != 0);
    return &session_->data;
}