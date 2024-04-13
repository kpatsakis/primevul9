static int compute_selinux_con_for_new_file(pid_t pid, int dir_fd, security_context_t *newcon)
{
    security_context_t srccon;
    security_context_t dstcon;

    const int r = is_selinux_enabled();
    if (r == 0)
    {
        *newcon = NULL;
        return 1;
    }
    else if (r == -1)
    {
        perror_msg("Couldn't get state of SELinux");
        return -1;
    }
    else if (r != 1)
        error_msg_and_die("Unexpected SELinux return value: %d", r);


    if (getpidcon_raw(pid, &srccon) < 0)
    {
        perror_msg("getpidcon_raw(%d)", pid);
        return -1;
    }

    if (fgetfilecon_raw(dir_fd, &dstcon) < 0)
    {
        perror_msg("getfilecon_raw(%s)", user_pwd);
        return -1;
    }

    if (security_compute_create_raw(srccon, dstcon, string_to_security_class("file"), newcon) < 0)
    {
        perror_msg("security_compute_create_raw(%s, %s, 'file')", srccon, dstcon);
        return -1;
    }

    return 0;
}