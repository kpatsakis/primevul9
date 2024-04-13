AbstractWebApplication::~AbstractWebApplication()
{
    // cleanup sessions data
    qDeleteAll(sessions_);
}