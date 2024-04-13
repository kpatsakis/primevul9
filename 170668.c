static void terminate(int sig)
{
/* #ifndef HAVE_ABSTRACT_SOCKETS */
    unlink(socket_file);
    exit(0);
/* #endif */
}