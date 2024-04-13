static void set_env( char* penv, const char* name )
{
    if( penv && *penv )
        g_setenv( name, penv, TRUE);
    else
        g_unsetenv( name );
}