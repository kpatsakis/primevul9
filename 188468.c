safe_open_or_create(const char *path, const char *mode, Error **errp)
{
    Error *local_err = NULL;
    int oflag;

    oflag = find_open_flag(mode, &local_err);
    if (local_err == NULL) {
        int fd;

        /* If the caller wants / allows creation of a new file, we implement it
         * with a two step process: open() + (open() / fchmod()).
         *
         * First we insist on creating the file exclusively as a new file. If
         * that succeeds, we're free to set any file-mode bits on it. (The
         * motivation is that we want to set those file-mode bits independently
         * of the current umask.)
         *
         * If the exclusive creation fails because the file already exists
         * (EEXIST is not possible for any other reason), we just attempt to
         * open the file, but in this case we won't be allowed to change the
         * file-mode bits on the preexistent file.
         *
         * The pathname should never disappear between the two open()s in
         * practice. If it happens, then someone very likely tried to race us.
         * In this case just go ahead and report the ENOENT from the second
         * open() to the caller.
         *
         * If the caller wants to open a preexistent file, then the first
         * open() is decisive and its third argument is ignored, and the second
         * open() and the fchmod() are never called.
         */
        fd = open(path, oflag | ((oflag & O_CREAT) ? O_EXCL : 0), 0);
        if (fd == -1 && errno == EEXIST) {
            oflag &= ~(unsigned)O_CREAT;
            fd = open(path, oflag);
        }

        if (fd == -1) {
            error_setg_errno(&local_err, errno, "failed to open file '%s' "
                             "(mode: '%s')", path, mode);
        } else {
            qemu_set_cloexec(fd);

            if ((oflag & O_CREAT) && fchmod(fd, DEFAULT_NEW_FILE_MODE) == -1) {
                error_setg_errno(&local_err, errno, "failed to set permission "
                                 "0%03o on new file '%s' (mode: '%s')",
                                 (unsigned)DEFAULT_NEW_FILE_MODE, path, mode);
            } else {
                FILE *f;

                f = fdopen(fd, mode);
                if (f == NULL) {
                    error_setg_errno(&local_err, errno, "failed to associate "
                                     "stdio stream with file descriptor %d, "
                                     "file '%s' (mode: '%s')", fd, path, mode);
                } else {
                    return f;
                }
            }

            close(fd);
            if (oflag & O_CREAT) {
                unlink(path);
            }
        }
    }

    error_propagate(errp, local_err);
    return NULL;
}