static void create_core_backtrace(pid_t tid, const char *executable, int signal_no, const char *dd_path)
{
#ifdef ENABLE_DUMP_TIME_UNWIND
    if (g_verbose > 1)
        sr_debug_parser = true;

    char *error_message = NULL;
    bool success = sr_abrt_create_core_stacktrace_from_core_hook(dd_path, tid, executable,
                                                                 signal_no, &error_message);

    if (!success)
    {
        log("Failed to create core_backtrace: %s", error_message);
        free(error_message);
    }
#endif /* ENABLE_DUMP_TIME_UNWIND */
}