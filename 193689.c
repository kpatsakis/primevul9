helptags_cb(char_u *fname, void *cookie)
{
    do_helptags(fname, *(int *)cookie, TRUE);
}