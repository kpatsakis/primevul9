bool AbstractWebApplication::isAuthNeeded()
{
    return (env_.clientAddress != QHostAddress::LocalHost
            && env_.clientAddress != QHostAddress::LocalHostIPv6
            && env_.clientAddress != QHostAddress("::ffff:127.0.0.1"))
            || Preferences::instance()->isWebUiLocalAuthEnabled();
}