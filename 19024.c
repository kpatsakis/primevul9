dnsc_shared_secrets_compfunc(void *m1, void *m2)
{
    return sodium_memcmp(m1, m2, DNSCRYPT_SHARED_SECRET_KEY_LENGTH);
}