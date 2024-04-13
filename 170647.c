static void get_socket_name( char* buf, int len )
{
    char* dpy = g_strdup(g_getenv("DISPLAY"));
    if(dpy && *dpy)
    {
        char* p = strchr(dpy, ':');
        for(++p; *p && *p != '.' && *p != '\n';)
            ++p;
        if(*p)
            *p = '\0';
    }
    /* NOTE: this socket name is incompatible with versions > 1.0.2,
            although this function is never used since 0.7.0 but
            libmenu-cache always requests exact socket name instead */
    g_snprintf( buf, len, "%s/.menu-cached-%s-%s", g_get_tmp_dir(),
                dpy ? dpy : ":0", g_get_user_name() );
    g_free(dpy);
}