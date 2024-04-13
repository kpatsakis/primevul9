bool AbstractWebApplication::isBanned() const
{
    return clientFailedAttempts_.value(env_.clientAddress, 0) >= MAX_AUTH_FAILED_ATTEMPTS;
}