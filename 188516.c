static void execute_async(DWORD WINAPI (*func)(LPVOID), LPVOID opaque,
                          Error **errp)
{
    Error *local_err = NULL;

    HANDLE thread = CreateThread(NULL, 0, func, opaque, 0, NULL);
    if (!thread) {
        error_setg(&local_err, QERR_QGA_COMMAND_FAILED,
                   "failed to dispatch asynchronous command");
        error_propagate(errp, local_err);
    }
}