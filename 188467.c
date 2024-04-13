static void transfer_vcpu(GuestLogicalProcessor *vcpu, bool sys2vcpu,
                          Error **errp)
{
    char *dirpath;
    int dirfd;

    dirpath = g_strdup_printf("/sys/devices/system/cpu/cpu%" PRId64 "/",
                              vcpu->logical_id);
    dirfd = open(dirpath, O_RDONLY | O_DIRECTORY);
    if (dirfd == -1) {
        error_setg_errno(errp, errno, "open(\"%s\")", dirpath);
    } else {
        static const char fn[] = "online";
        int fd;
        int res;

        fd = openat(dirfd, fn, sys2vcpu ? O_RDONLY : O_RDWR);
        if (fd == -1) {
            if (errno != ENOENT) {
                error_setg_errno(errp, errno, "open(\"%s/%s\")", dirpath, fn);
            } else if (sys2vcpu) {
                vcpu->online = true;
                vcpu->can_offline = false;
            } else if (!vcpu->online) {
                error_setg(errp, "logical processor #%" PRId64 " can't be "
                           "offlined", vcpu->logical_id);
            } /* otherwise pretend successful re-onlining */
        } else {
            unsigned char status;

            res = pread(fd, &status, 1, 0);
            if (res == -1) {
                error_setg_errno(errp, errno, "pread(\"%s/%s\")", dirpath, fn);
            } else if (res == 0) {
                error_setg(errp, "pread(\"%s/%s\"): unexpected EOF", dirpath,
                           fn);
            } else if (sys2vcpu) {
                vcpu->online = (status != '0');
                vcpu->can_offline = true;
            } else if (vcpu->online != (status != '0')) {
                status = '0' + vcpu->online;
                if (pwrite(fd, &status, 1, 0) == -1) {
                    error_setg_errno(errp, errno, "pwrite(\"%s/%s\")", dirpath,
                                     fn);
                }
            } /* otherwise pretend successful re-(on|off)-lining */

            res = close(fd);
            g_assert(res == 0);
        }

        res = close(dirfd);
        g_assert(res == 0);
    }

    g_free(dirpath);
}