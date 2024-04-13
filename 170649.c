guint32 menu_cache_get_desktop_env_flag( MenuCache* cache, const char* desktop_env )
{
    char** de;
    char **envs;
    guint32 flags = 0;
    int j;

    if (desktop_env == NULL || desktop_env[0] == '\0')
        return flags;

    envs = g_strsplit(desktop_env, ":", -1);
    MENU_CACHE_LOCK;
    de = cache->known_des;
    for (j = 0; envs[j]; j++)
    {
        if( de )
        {
            int i;
            for( i = 0; de[i]; ++i )
                if (strcmp(envs[j], de[i]) == 0)
                    break;
            if (de[i])
            {
                flags |= 1 << (i + N_KNOWN_DESKTOPS);
                continue;
            }
        }
        if (strcmp(envs[j], "GNOME") == 0)
            flags |= SHOW_IN_GNOME;
        else if (strcmp(envs[j], "KDE") == 0)
            flags |= SHOW_IN_KDE;
        else if (strcmp(envs[j], "XFCE") == 0)
            flags |= SHOW_IN_XFCE;
        else if (strcmp(envs[j], "LXDE") == 0)
            flags |= SHOW_IN_LXDE;
        else if (strcmp(envs[j], "ROX") == 0)
            flags |= SHOW_IN_ROX;
    }
    MENU_CACHE_UNLOCK;
    g_strfreev(envs);
    return flags;
}