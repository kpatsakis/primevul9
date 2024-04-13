InstallSignalHandlers(void)
{
    /*
     * Install signal handler for unexpected signals
     */
    xf86Info.caughtSignal = FALSE;
    if (!xf86Info.notrapSignals) {
        OsRegisterSigWrapper(xf86SigWrapper);
    }
    else {
        signal(SIGSEGV, SIG_DFL);
        signal(SIGILL, SIG_DFL);
#ifdef SIGEMT
        signal(SIGEMT, SIG_DFL);
#endif
        signal(SIGFPE, SIG_DFL);
        signal(SIGBUS, SIG_DFL);
        signal(SIGSYS, SIG_DFL);
        signal(SIGXCPU, SIG_DFL);
        signal(SIGXFSZ, SIG_DFL);
    }
}