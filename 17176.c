gettail_sep(char_u *fname)
{
    char_u	*p;
    char_u	*t;

    p = get_past_head(fname);	// don't remove the '/' from "c:/file"
    t = gettail(fname);
    while (t > p && after_pathsep(fname, t))
	--t;
#ifdef VMS
    // path separator is part of the path
    ++t;
#endif
    return t;
}