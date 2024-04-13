static int omode_to_uflags(int8_t mode)
{
    int ret = 0;

    switch (mode & 3) {
    case Oread:
        ret = O_RDONLY;
        break;
    case Ordwr:
        ret = O_RDWR;
        break;
    case Owrite:
        ret = O_WRONLY;
        break;
    case Oexec:
        ret = O_RDONLY;
        break;
    }

    if (mode & Otrunc) {
        ret |= O_TRUNC;
    }

    if (mode & Oappend) {
        ret |= O_APPEND;
    }

    if (mode & Oexcl) {
        ret |= O_EXCL;
    }

    return ret;
}