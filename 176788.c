int save_crashing_binary(pid_t pid, const char *dest_path, uid_t uid, gid_t gid)
{
    char buf[sizeof("/proc/%lu/exe") + sizeof(long)*3];

    sprintf(buf, "/proc/%lu/exe", (long)pid);
    int src_fd_binary = open(buf, O_RDONLY); /* might fail and return -1, it's ok */
    if (src_fd_binary < 0)
    {
        log_notice("Failed to open an image of crashing binary");
        return 0;
    }

    int dst_fd = open(dest_path, O_WRONLY | O_CREAT | O_TRUNC, DEFAULT_DUMP_DIR_MODE);
    if (dst_fd < 0)
    {
        log_notice("Failed to create file '%s'", dest_path);
        close(src_fd_binary);
        return -1;
    }

    IGNORE_RESULT(fchown(dst_fd, uid, gid));

    off_t sz = copyfd_eof(src_fd_binary, dst_fd, COPYFD_SPARSE);
    close(src_fd_binary);

    return fsync(dst_fd) != 0 || close(dst_fd) != 0 || sz < 0;
}