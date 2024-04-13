get_pubkey_next (GETKEY_CTX ctx, PKT_public_key * pk, KBNODE * ret_keyblock)
{
  int rc;

  rc = lookup (ctx, ret_keyblock, 0);
  if (!rc && pk && ret_keyblock)
    pk_from_block (ctx, pk, *ret_keyblock);

  return rc;
}