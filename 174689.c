TrapSignals(void)
{
    if (xf86Info.notrapSignals) {
        OsSignal(SIGSEGV, SIG_DFL);
        OsSignal(SIGABRT, SIG_DFL);
        OsSignal(SIGILL, SIG_DFL);
#ifdef SIGEMT
        OsSignal(SIGEMT, SIG_DFL);
#endif
        OsSignal(SIGFPE, SIG_DFL);
        OsSignal(SIGBUS, SIG_DFL);
        OsSignal(SIGSYS, SIG_DFL);
        OsSignal(SIGXCPU, SIG_DFL);
        OsSignal(SIGXFSZ, SIG_DFL);
    }
}