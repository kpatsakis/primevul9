token_clear(directory_token_t *tok)
{
  if (tok->key)
    crypto_free_pk_env(tok->key);
}