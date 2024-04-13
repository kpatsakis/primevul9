static void free_sockent_server(struct sockent_server *ses) /* {{{ */
{
  for (size_t i = 0; i < ses->fd_num; i++) {
    if (ses->fd[i] >= 0) {
      close(ses->fd[i]);
      ses->fd[i] = -1;
    }
  }

  sfree(ses->fd);
#if HAVE_LIBGCRYPT
  sfree(ses->auth_file);
  fbh_destroy(ses->userdb);
  if (ses->cypher != NULL)
    gcry_cipher_close(ses->cypher);
#endif
} /* }}} void free_sockent_server */