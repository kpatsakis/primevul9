static int get_dotl_openflags(V9fsState *s, int oflags)
{
    int flags;
    /*
     * Filter the client open flags
     */
    flags = dotl_to_open_flags(oflags);
    flags &= ~(O_NOCTTY | O_ASYNC | O_CREAT);
    /*
     * Ignore direct disk access hint until the server supports it.
     */
    flags &= ~O_DIRECT;
    return flags;
}