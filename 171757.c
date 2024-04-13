on_monitor_signal(int signo)
{
    signal_received = signo;

#ifdef POSIX_SIGTYPE
    return;
#else
    return(0);
#endif
}