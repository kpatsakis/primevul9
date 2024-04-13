_gcry_rngcsprng_set_daemon_socket (const char *socketname)
{
#ifdef USE_RANDOM_DAEMON
  if (daemon_socket_name)
    BUG ();

  daemon_socket_name = gcry_xstrdup (socketname);
#else /*!USE_RANDOM_DAEMON*/
  (void)socketname;
#endif /*!USE_RANDOM_DAEMON*/
}