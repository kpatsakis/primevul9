int lcc_network_buffer_initialize (lcc_network_buffer_t *nb) /* {{{ */
{
  if (nb == NULL)
    return (EINVAL);

  memset (nb->buffer, 0, nb->size);
  memset (&nb->state, 0, sizeof (nb->state));
  nb->ptr = nb->buffer;
  nb->free = nb->size;

#if HAVE_LIBGCRYPT
  if (nb->seclevel == SIGN)
  {
    size_t username_len;
    uint16_t pkg_type = htons (TYPE_SIGN_SHA256);
    uint16_t pkg_length = PART_SIGNATURE_SHA256_SIZE;

    assert (nb->username != NULL);
    username_len = strlen (nb->username);
    pkg_length = htons (pkg_length + ((uint16_t) username_len));

    /* Fill in everything but the hash value here. */
    memcpy (nb->ptr, &pkg_type, sizeof (pkg_type));
    memcpy (nb->ptr + sizeof (pkg_type), &pkg_length, sizeof (pkg_length));
    nb->ptr += PART_SIGNATURE_SHA256_SIZE;
    nb->free -= PART_SIGNATURE_SHA256_SIZE;

    memcpy (nb->ptr, nb->username, username_len);
    nb->ptr += username_len;
    nb->free -= username_len;
  }
  else if (nb->seclevel == ENCRYPT)
  {
    size_t username_length = strlen (nb->username);
    uint16_t pkg_type = htons (TYPE_ENCR_AES256);
    uint16_t pkg_length = 0; /* Filled in in finalize. */
    uint16_t pkg_user_len = htons ((uint16_t) username_length);
    char hash[20];

    nb->encr_header_len = username_length;
    nb->encr_header_len += PART_ENCRYPTION_AES256_SIZE;

    gcry_randomize ((void *) &nb->encr_iv, sizeof (nb->encr_iv),
        GCRY_STRONG_RANDOM);

    /* Filled in in finalize. */
    memset (hash, 0, sizeof (hash));

    ADD_STATIC (nb, pkg_type);
    ADD_STATIC (nb, pkg_length);
    ADD_STATIC (nb, pkg_user_len);
    ADD_GENERIC (nb, nb->username, username_length);
    ADD_GENERIC (nb, nb->encr_iv, sizeof (nb->encr_iv));
    ADD_GENERIC (nb, hash, sizeof (hash));
    assert ((nb->encr_header_len + nb->free) == nb->size);
  }
#endif

  return (0);
} /* }}} int lcc_network_buffer_initialize */