xf86SetLogVerbosity(int verb)
{
    int save = xf86LogVerbose;

    xf86LogVerbose = verb;
    LogSetParameter(XLOG_FILE_VERBOSITY, verb);
    return save;
}