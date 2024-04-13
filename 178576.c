static int sh_init(struct curl_hash *hash, int hashsize)
{
  return Curl_hash_init(hash, hashsize, hash_fd, fd_key_compare,
                        sh_freeentry);
}