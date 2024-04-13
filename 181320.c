static int dotl_to_open_flags(int flags)
{
    int i;
    /*
     * We have same bits for P9_DOTL_READONLY, P9_DOTL_WRONLY
     * and P9_DOTL_NOACCESS
     */
    int oflags = flags & O_ACCMODE;

    DotlOpenflagMap dotl_oflag_map[] = {
        { P9_DOTL_CREATE, O_CREAT },
        { P9_DOTL_EXCL, O_EXCL },
        { P9_DOTL_NOCTTY , O_NOCTTY },
        { P9_DOTL_TRUNC, O_TRUNC },
        { P9_DOTL_APPEND, O_APPEND },
        { P9_DOTL_NONBLOCK, O_NONBLOCK } ,
        { P9_DOTL_DSYNC, O_DSYNC },
        { P9_DOTL_FASYNC, FASYNC },
        { P9_DOTL_DIRECT, O_DIRECT },
        { P9_DOTL_LARGEFILE, O_LARGEFILE },
        { P9_DOTL_DIRECTORY, O_DIRECTORY },
        { P9_DOTL_NOFOLLOW, O_NOFOLLOW },
        { P9_DOTL_NOATIME, O_NOATIME },
        { P9_DOTL_SYNC, O_SYNC },
    };

    for (i = 0; i < ARRAY_SIZE(dotl_oflag_map); i++) {
        if (flags & dotl_oflag_map[i].dotl_flag) {
            oflags |= dotl_oflag_map[i].open_flag;
        }
    }

    return oflags;
}