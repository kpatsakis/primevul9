AbstractWebApplication::AbstractWebApplication(QObject *parent)
    : Http::ResponseBuilder(parent)
    , session_(0)
{
    QTimer *timer = new QTimer(this);
    timer->setInterval(60000); // 1 min.
    connect(timer, SIGNAL(timeout()), SLOT(removeInactiveSessions()));
}