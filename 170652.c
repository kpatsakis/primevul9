static void pre_exec( gpointer user_data )
{
    char** env = (char**)user_data;
    set_env(*env, "XDG_CACHE_HOME");
    set_env(*++env, "XDG_CONFIG_DIRS");
    set_env(*++env, "XDG_MENU_PREFIX");
    set_env(*++env, "XDG_DATA_DIRS");
    set_env(*++env, "XDG_CONFIG_HOME");
    set_env(*++env, "XDG_DATA_HOME");
    set_env(*++env, "CACHE_GEN_VERSION");
}