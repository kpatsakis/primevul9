static void __attribute__((__constructor__)) v9fs_set_fd_limit(void)
{
    struct rlimit rlim;
    if (getrlimit(RLIMIT_NOFILE, &rlim) < 0) {
        error_report("Failed to get the resource limit");
        exit(1);
    }
    open_fd_hw = rlim.rlim_cur - MIN(400, rlim.rlim_cur / 3);
    open_fd_rc = rlim.rlim_cur / 2;
}