int main(int argc, char** argv)
{
    /* Kernel starts us with all fd's closed.
     * But it's dangerous:
     * fprintf(stderr) can dump messages into random fds, etc.
     * Ensure that if any of fd 0,1,2 is closed, we open it to /dev/null.
     */
    int fd = xopen("/dev/null", O_RDWR);
    while (fd < 2)
        fd = xdup(fd);
    if (fd > 2)
        close(fd);

    logmode = LOGMODE_JOURNAL;

    /* Parse abrt.conf */
    load_abrt_conf();
    /* ... and plugins/CCpp.conf */
    bool setting_MakeCompatCore;
    bool setting_SaveBinaryImage;
    bool setting_SaveFullCore;
    bool setting_CreateCoreBacktrace;
    {
        map_string_t *settings = new_map_string();
        load_abrt_plugin_conf_file("CCpp.conf", settings);
        const char *value;
        value = get_map_string_item_or_NULL(settings, "MakeCompatCore");
        setting_MakeCompatCore = value && string_to_bool(value);
        value = get_map_string_item_or_NULL(settings, "SaveBinaryImage");
        setting_SaveBinaryImage = value && string_to_bool(value);
        value = get_map_string_item_or_NULL(settings, "SaveFullCore");
        setting_SaveFullCore = value ? string_to_bool(value) : true;
        value = get_map_string_item_or_NULL(settings, "CreateCoreBacktrace");
        setting_CreateCoreBacktrace = value ? string_to_bool(value) : true;
        value = get_map_string_item_or_NULL(settings, "VerboseLog");
        if (value)
            g_verbose = xatoi_positive(value);
        free_map_string(settings);
    }

    if (argc == 2 && strcmp(argv[1], "--config-test"))
        return test_configuration(setting_SaveFullCore, setting_CreateCoreBacktrace);

    if (argc < 8)
    {
        /* percent specifier:         %s   %c              %p  %u  %g  %t   %e          %P         %i*/
        /* argv:                  [0] [1]  [2]             [3] [4] [5] [6]  [7]         [8]        [9]*/
        error_msg_and_die("Usage: %s SIGNO CORE_SIZE_LIMIT PID UID GID TIME BINARY_NAME GLOBAL_PID [TID]", argv[0]);
    }

    /* Not needed on 2.6.30.
     * At least 2.6.18 has a bug where
     * argv[1] = "SIGNO CORE_SIZE_LIMIT PID ..."
     * argv[2] = "CORE_SIZE_LIMIT PID ..."
     * and so on. Fixing it:
     */
    if (strchr(argv[1], ' '))
    {
        int i;
        for (i = 1; argv[i]; i++)
        {
            strchrnul(argv[i], ' ')[0] = '\0';
        }
    }

    errno = 0;
    const char* signal_str = argv[1];
    int signal_no = xatoi_positive(signal_str);
    off_t ulimit_c = strtoull(argv[2], NULL, 10);
    if (ulimit_c < 0) /* unlimited? */
    {
        /* set to max possible >0 value */
        ulimit_c = ~((off_t)1 << (sizeof(off_t)*8-1));
    }
    const char *pid_str = argv[3];
    pid_t local_pid = xatoi_positive(argv[3]);
    uid_t uid = xatoi_positive(argv[4]);
    if (errno || local_pid <= 0)
    {
        perror_msg_and_die("PID '%s' or limit '%s' is bogus", argv[3], argv[2]);
    }

    {
        char *s = xmalloc_fopen_fgetline_fclose(VAR_RUN"/abrt/saved_core_pattern");
        /* If we have a saved pattern and it's not a "|PROG ARGS" thing... */
        if (s && s[0] != '|')
            core_basename = s;
        else
            free(s);
    }
    const char *global_pid_str = argv[8];
    pid_t pid = xatoi_positive(argv[8]);

    pid_t tid = 0;
    if (argv[9])
    {
        tid = xatoi_positive(argv[8]);
    }

    char path[PATH_MAX];

    char *executable = get_executable(pid);
    if (executable && strstr(executable, "/abrt-hook-ccpp"))
    {
        error_msg_and_die("PID %lu is '%s', not dumping it to avoid recursion",
                        (long)pid, executable);
    }

    user_pwd = get_cwd(pid); /* may be NULL on error */
    log_notice("user_pwd:'%s'", user_pwd);

    sprintf(path, "/proc/%lu/status", (long)pid);
    char *proc_pid_status = xmalloc_xopen_read_close(path, /*maxsz:*/ NULL);

    uid_t fsuid = uid;
    uid_t tmp_fsuid = get_fsuid(proc_pid_status);
    if (tmp_fsuid < 0)
        perror_msg_and_die("Can't parse 'Uid: line' in /proc/%lu/status", (long)pid);

    int suid_policy = dump_suid_policy();
    if (tmp_fsuid != uid)
    {
        /* use root for suided apps unless it's explicitly set to UNSAFE */
        fsuid = 0;
        if (suid_policy == DUMP_SUID_UNSAFE)
        {
            fsuid = tmp_fsuid;
        }
    }

    /* Open a fd to compat coredump, if requested and is possible */
    int user_core_fd = -1;
    if (setting_MakeCompatCore && ulimit_c != 0)
        /* note: checks "user_pwd == NULL" inside; updates core_basename */
        user_core_fd = open_user_core(uid, fsuid, pid, &argv[1]);

    if (executable == NULL)
    {
        /* readlink on /proc/$PID/exe failed, don't create abrt dump dir */
        error_msg("Can't read /proc/%lu/exe link", (long)pid);
        return create_user_core(user_core_fd, pid, ulimit_c);
    }

    const char *signame = NULL;
    if (!signal_is_fatal(signal_no, &signame))
        return create_user_core(user_core_fd, pid, ulimit_c); // not a signal we care about

    if (!daemon_is_ok())
    {
        /* not an error, exit with exit code 0 */
        log("abrtd is not running. If it crashed, "
            "/proc/sys/kernel/core_pattern contains a stale value, "
            "consider resetting it to 'core'"
        );
        return create_user_core(user_core_fd, pid, ulimit_c);
    }

    if (g_settings_nMaxCrashReportsSize > 0)
    {
        /* If free space is less than 1/4 of MaxCrashReportsSize... */
        if (low_free_space(g_settings_nMaxCrashReportsSize, g_settings_dump_location))
            return create_user_core(user_core_fd, pid, ulimit_c);
    }

    /* Check /var/tmp/abrt/last-ccpp marker, do not dump repeated crashes
     * if they happen too often. Else, write new marker value.
     */
    snprintf(path, sizeof(path), "%s/last-ccpp", g_settings_dump_location);
    if (check_recent_crash_file(path, executable))
    {
        /* It is a repeating crash */
        return create_user_core(user_core_fd, pid, ulimit_c);
    }

    const char *last_slash = strrchr(executable, '/');
    if (last_slash && strncmp(++last_slash, "abrt", 4) == 0)
    {
        /* If abrtd/abrt-foo crashes, we don't want to create a _directory_,
         * since that can make new copy of abrtd to process it,
         * and maybe crash again...
         * Unlike dirs, mere files are ignored by abrtd.
         */
        snprintf(path, sizeof(path), "%s/%s-coredump", g_settings_dump_location, last_slash);
        int abrt_core_fd = xopen3(path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        off_t core_size = copyfd_eof(STDIN_FILENO, abrt_core_fd, COPYFD_SPARSE);
        if (core_size < 0 || fsync(abrt_core_fd) != 0)
        {
            unlink(path);
            /* copyfd_eof logs the error including errno string,
             * but it does not log file name */
            error_msg_and_die("Error saving '%s'", path);
        }
        log_notice("Saved core dump of pid %lu (%s) to %s (%llu bytes)", (long)pid, executable, path, (long long)core_size);
        return 0;
    }

    unsigned path_len = snprintf(path, sizeof(path), "%s/ccpp-%s-%lu.new",
            g_settings_dump_location, iso_date_string(NULL), (long)pid);
    if (path_len >= (sizeof(path) - sizeof("/"FILENAME_COREDUMP)))
    {
        return create_user_core(user_core_fd, pid, ulimit_c);
    }

    /* use fsuid instead of uid, so we don't expose any sensitive
     * information of suided app in /var/tmp/abrt
     */
    dd = dd_create(path, fsuid, DEFAULT_DUMP_DIR_MODE);
    if (dd)
    {
        char source_filename[sizeof("/proc/%lu/somewhat_long_name") + sizeof(long)*3];
        int source_base_ofs = sprintf(source_filename, "/proc/%lu/root", (long)pid);
        source_base_ofs -= strlen("root");

        /* What's wrong on using /proc/[pid]/root every time ?*/
        /* It creates os_info_in_root_dir for all crashes. */
        char *rootdir = process_has_own_root(pid) ? get_rootdir(pid) : NULL;
        /* Yes, test 'rootdir' but use 'source_filename' because 'rootdir' can
         * be '/' for a process with own namespace. 'source_filename' is /proc/[pid]/root. */
        dd_create_basic_files(dd, fsuid, (rootdir != NULL) ? source_filename : NULL);

        char *dest_filename = concat_path_file(dd->dd_dirname, "also_somewhat_longish_name");
        char *dest_base = strrchr(dest_filename, '/') + 1;

        // Disabled for now: /proc/PID/smaps tends to be BIG,
        // and not much more informative than /proc/PID/maps:
        //copy_file(source_filename, dest_filename, 0640);
        //chown(dest_filename, dd->dd_uid, dd->dd_gid);

        strcpy(source_filename + source_base_ofs, "maps");
        strcpy(dest_base, FILENAME_MAPS);
        copy_file(source_filename, dest_filename, DEFAULT_DUMP_DIR_MODE);
        IGNORE_RESULT(chown(dest_filename, dd->dd_uid, dd->dd_gid));

        strcpy(source_filename + source_base_ofs, "limits");
        strcpy(dest_base, FILENAME_LIMITS);
        copy_file(source_filename, dest_filename, DEFAULT_DUMP_DIR_MODE);
        IGNORE_RESULT(chown(dest_filename, dd->dd_uid, dd->dd_gid));

        strcpy(source_filename + source_base_ofs, "cgroup");
        strcpy(dest_base, FILENAME_CGROUP);
        copy_file(source_filename, dest_filename, DEFAULT_DUMP_DIR_MODE);
        IGNORE_RESULT(chown(dest_filename, dd->dd_uid, dd->dd_gid));

        strcpy(source_filename + source_base_ofs, "mountinfo");
        strcpy(dest_base, FILENAME_MOUNTINFO);
        copy_file(source_filename, dest_filename, DEFAULT_DUMP_DIR_MODE);
        IGNORE_RESULT(chown(dest_filename, dd->dd_uid, dd->dd_gid));

        strcpy(dest_base, FILENAME_OPEN_FDS);
        strcpy(source_filename + source_base_ofs, "fd");
        if (dump_fd_info(dest_filename, source_filename) == 0)
            IGNORE_RESULT(chown(dest_filename, dd->dd_uid, dd->dd_gid));

        strcpy(dest_base, FILENAME_NAMESPACES);
        if (dump_namespace_diff(dest_filename, 1, pid))
            IGNORE_RESULT(chown(dest_filename, dd->dd_uid, dd->dd_gid));

        free(dest_filename);

        char *tmp = NULL;
        get_env_variable(pid, "container", &tmp);
        if (tmp != NULL)
        {
            dd_save_text(dd, FILENAME_CONTAINER, tmp);
            free(tmp);
            tmp = NULL;
        }

        get_env_variable(pid, "container_uuid", &tmp);
        if (tmp != NULL)
        {
            dd_save_text(dd, FILENAME_CONTAINER_UUID, tmp);
            free(tmp);
        }

        /* There's no need to compare mount namespaces and search for '/' in
         * mountifo.  Comparison of inodes of '/proc/[pid]/root' and '/' works
         * fine. If those inodes do not equal each other, we have to verify
         * that '/proc/[pid]/root' is not a symlink to a chroot.
         */
        const int containerized = (rootdir != NULL && strcmp(rootdir, "/") == 0);
        if (containerized)
        {
            log_debug("Process %d is considered to be containerized", pid);
            pid_t container_pid;
            if (get_pid_of_container(pid, &container_pid) == 0)
            {
                char *container_cmdline = get_cmdline(container_pid);
                dd_save_text(dd, FILENAME_CONTAINER_CMDLINE, container_cmdline);
                free(container_cmdline);
            }
        }

        dd_save_text(dd, FILENAME_ANALYZER, "abrt-ccpp");
        dd_save_text(dd, FILENAME_TYPE, "CCpp");
        dd_save_text(dd, FILENAME_EXECUTABLE, executable);
        dd_save_text(dd, FILENAME_PID, pid_str);
        dd_save_text(dd, FILENAME_GLOBAL_PID, global_pid_str);
        dd_save_text(dd, FILENAME_PROC_PID_STATUS, proc_pid_status);
        if (user_pwd)
            dd_save_text(dd, FILENAME_PWD, user_pwd);

        if (rootdir)
        {
            if (strcmp(rootdir, "/") != 0)
                dd_save_text(dd, FILENAME_ROOTDIR, rootdir);
        }
        free(rootdir);

        char *reason = xasprintf("%s killed by SIG%s",
                                 last_slash, signame ? signame : signal_str);
        dd_save_text(dd, FILENAME_REASON, reason);
        free(reason);

        char *cmdline = get_cmdline(pid);
        dd_save_text(dd, FILENAME_CMDLINE, cmdline ? : "");
        free(cmdline);

        char *environ = get_environ(pid);
        dd_save_text(dd, FILENAME_ENVIRON, environ ? : "");
        free(environ);

        char *fips_enabled = xmalloc_fopen_fgetline_fclose("/proc/sys/crypto/fips_enabled");
        if (fips_enabled)
        {
            if (strcmp(fips_enabled, "0") != 0)
                dd_save_text(dd, "fips_enabled", fips_enabled);
            free(fips_enabled);
        }

        dd_save_text(dd, FILENAME_ABRT_VERSION, VERSION);

        if (setting_SaveBinaryImage)
        {
            strcpy(path + path_len, "/"FILENAME_BINARY);

            if (save_crashing_binary(pid, path, dd->dd_uid, dd->dd_gid))
            {
                error_msg("Error saving '%s'", path);

                goto error_exit;
            }
        }

        off_t core_size = 0;
        if (setting_SaveFullCore)
        {
            strcpy(path + path_len, "/"FILENAME_COREDUMP);
            int abrt_core_fd = create_or_die(path, user_core_fd);

            /* We write both coredumps at once.
             * We can't write user coredump first, since it might be truncated
             * and thus can't be copied and used as abrt coredump;
             * and if we write abrt coredump first and then copy it as user one,
             * then we have a race when process exits but coredump does not exist yet:
             * $ echo -e '#include<signal.h>\nmain(){raise(SIGSEGV);}' | gcc -o test -x c -
             * $ rm -f core*; ulimit -c unlimited; ./test; ls -l core*
             * 21631 Segmentation fault (core dumped) ./test
             * ls: cannot access core*: No such file or directory <=== BAD
             */
            core_size = copyfd_sparse(STDIN_FILENO, abrt_core_fd, user_core_fd, ulimit_c);
            if (fsync(abrt_core_fd) != 0 || close(abrt_core_fd) != 0 || core_size < 0)
            {
                unlink(path);

                /* copyfd_sparse logs the error including errno string,
                 * but it does not log file name */
                error_msg("Error writing '%s'", path);

                goto error_exit;
            }
            if (user_core_fd >= 0
                /* error writing user coredump? */
             && (fsync(user_core_fd) != 0 || close(user_core_fd) != 0
                /* user coredump is too big? */
                || (ulimit_c == 0 /* paranoia */ || core_size > ulimit_c)
                )
            ) {
                /* nuke it (silently) */
                xchdir(user_pwd);
                unlink(core_basename);
            }
        }
        else
        {
            /* User core is created even if WriteFullCore is off. */
            create_user_core(user_core_fd, pid, ulimit_c);
        }

        /* User core is either written or closed */
        user_core_fd = -1;

        /*
         * ! No other errors should cause removal of the user core !
         */

        /* Save JVM crash log if it exists. (JVM's coredump per se
         * is nearly useless for JVM developers)
         */
        {
            char *java_log = xasprintf("/tmp/jvm-%lu/hs_error.log", (long)pid);
            int src_fd = open(java_log, O_RDONLY);
            free(java_log);

            /* If we couldn't open the error log in /tmp directory we can try to
             * read the log from the current directory. It may produce AVC, it
             * may produce some error log but all these are expected.
             */
            if (src_fd < 0)
            {
                java_log = xasprintf("%s/hs_err_pid%lu.log", user_pwd, (long)pid);
                src_fd = open(java_log, O_RDONLY);
                free(java_log);
            }

            if (src_fd >= 0)
            {
                strcpy(path + path_len, "/hs_err.log");
                int dst_fd = create_or_die(path, user_core_fd);
                off_t sz = copyfd_eof(src_fd, dst_fd, COPYFD_SPARSE);
                if (close(dst_fd) != 0 || sz < 0)
                {
                    error_msg("Error saving '%s'", path);

                    goto error_exit;
                }
                close(src_fd);
            }
        }

        /* Perform crash-time unwind of the guilty thread. */
        if (tid > 0 && setting_CreateCoreBacktrace)
            create_core_backtrace(tid, executable, signal_no, dd->dd_dirname);

        /* We close dumpdir before we start catering for crash storm case.
         * Otherwise, delete_dump_dir's from other concurrent
         * CCpp's won't be able to delete our dump (their delete_dump_dir
         * will wait for us), and we won't be able to delete their dumps.
         * Classic deadlock.
         */
        dd_close(dd);
        dd = NULL;

        path[path_len] = '\0'; /* path now contains only directory name */
        char *newpath = xstrndup(path, path_len - (sizeof(".new")-1));
        if (rename(path, newpath) == 0)
            strcpy(path, newpath);
        free(newpath);

        if (core_size > 0)
            log_notice("Saved core dump of pid %lu (%s) to %s (%llu bytes)",
                       (long)pid, executable, path, (long long)core_size);

        notify_new_path(path);

        /* rhbz#539551: "abrt going crazy when crashing process is respawned" */
        if (g_settings_nMaxCrashReportsSize > 0)
        {
            /* x1.25 and round up to 64m: go a bit up, so that usual in-daemon trimming
             * kicks in first, and we don't "fight" with it:
             */
            unsigned maxsize = g_settings_nMaxCrashReportsSize + g_settings_nMaxCrashReportsSize / 4;
            maxsize |= 63;
            trim_problem_dirs(g_settings_dump_location, maxsize * (double)(1024*1024), path);
        }

        return 0;
    }

    /* We didn't create abrt dump, but may need to create compat coredump */
    return create_user_core(user_core_fd, pid, ulimit_c);

error_exit:
    if (dd)
        dd_delete(dd);

    if (user_core_fd >= 0)
    {
        xchdir(user_pwd);
        unlink(core_basename);
    }

    xfunc_die();
}