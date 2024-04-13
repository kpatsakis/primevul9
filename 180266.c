static void free_sockent_client(struct sockent_client *sec) /* {{{ */
{
  if (sec->fd >= 0) {
    close(sec->fd);
    sec->fd = -1;
  }
  sfree(sec->addr);
#if HAVE_LIBGCRYPT
  sfree(sec->username);
  sfree(sec->password);
  if (sec->cypher != NULL)
    gcry_cipher_close(sec->cypher);
#endif
} /* }}} void free_sockent_client */