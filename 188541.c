static void handle_set_nonblocking(HANDLE fh)
{
    DWORD file_type, pipe_state;
    file_type = GetFileType(fh);
    if (file_type != FILE_TYPE_PIPE) {
        return;
    }
    /* If file_type == FILE_TYPE_PIPE, according to MSDN
     * the specified file is socket or named pipe */
    if (!GetNamedPipeHandleState(fh, &pipe_state, NULL,
                                 NULL, NULL, NULL, 0)) {
        return;
    }
    /* The fd is named pipe fd */
    if (pipe_state & PIPE_NOWAIT) {
        return;
    }

    pipe_state |= PIPE_NOWAIT;
    SetNamedPipeHandleState(fh, &pipe_state, NULL, NULL);
}