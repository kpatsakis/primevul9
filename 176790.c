static int create_user_core(int user_core_fd, pid_t pid, off_t ulimit_c)
{
    if (user_core_fd >= 0)
    {
        off_t core_size = copyfd_size(STDIN_FILENO, user_core_fd, ulimit_c, COPYFD_SPARSE);
        if (fsync(user_core_fd) != 0 || close(user_core_fd) != 0 || core_size < 0)
        {
            /* perror first, otherwise unlink may trash errno */
            perror_msg("Error writing '%s'", full_core_basename);
            xchdir(user_pwd);
            unlink(core_basename);
            return 1;
        }
        if (ulimit_c == 0 || core_size > ulimit_c)
        {
            xchdir(user_pwd);
            unlink(core_basename);
            return 1;
        }
        log_notice("Saved core dump of pid %lu to %s (%llu bytes)", (long)pid, full_core_basename, (long long)core_size);
    }

    return 0;
}