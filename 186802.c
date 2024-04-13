void AbstractWebApplication::resetFailedAttempts()
{
    clientFailedAttempts_.remove(env_.clientAddress);
}