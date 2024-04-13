term_bg_default(void)
{
#if defined(WIN3264)
    /* DOS console nearly always black */
    return (char_u *)"dark";
#else
    char_u	*p;

    if (STRCMP(T_NAME, "linux") == 0
	    || STRCMP(T_NAME, "screen.linux") == 0
	    || STRCMP(T_NAME, "cygwin") == 0
	    || STRCMP(T_NAME, "putty") == 0
	    || ((p = mch_getenv((char_u *)"COLORFGBG")) != NULL
		&& (p = vim_strrchr(p, ';')) != NULL
		&& ((p[1] >= '0' && p[1] <= '6') || p[1] == '8')
		&& p[2] == NUL))
	return (char_u *)"dark";
    return (char_u *)"light";
#endif
}