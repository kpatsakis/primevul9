size_t Curl_mbedtls_version(char *buffer, size_t size)
{
  unsigned int version = mbedtls_version_get_number();
  return snprintf(buffer, size, "mbedTLS/%d.%d.%d", version>>24,
                  (version>>16)&0xff, (version>>8)&0xff);
}