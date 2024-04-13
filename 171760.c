on_monitor_sighup(int signo)
{
    sighup_received = 1;

#ifdef POSIX_SIGTYPE
    return;
#else
    return(0);
#endif
}