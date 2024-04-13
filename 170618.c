static gboolean on_server_conn_close(GIOChannel* ch, GIOCondition cond, gpointer user_data)
{
    /* FIXME: is this possible? */
    /* the server socket is accidentally closed. terminate the server. */
    terminate(SIGTERM);
    return TRUE;
}