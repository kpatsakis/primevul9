cert_cb (void *opaque, const unsigned char *cert, size_t certlen)
{
  printf ("got a certificate of %u bytes length\n", certlen);
}