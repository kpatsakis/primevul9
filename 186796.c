int AbstractWebApplication::failedAttempts() const
{
    return clientFailedAttempts_.value(env_.clientAddress, 0);
}