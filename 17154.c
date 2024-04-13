getftypest(stat_T *st)
{
    char    *t;

    if (S_ISREG(st->st_mode))
	t = "file";
    else if (S_ISDIR(st->st_mode))
	t = "dir";
    else if (S_ISLNK(st->st_mode))
	t = "link";
    else if (S_ISBLK(st->st_mode))
	t = "bdev";
    else if (S_ISCHR(st->st_mode))
	t = "cdev";
    else if (S_ISFIFO(st->st_mode))
	t = "fifo";
    else if (S_ISSOCK(st->st_mode))
	t = "socket";
    else
	t = "other";
    return (char_u*)t;
}