directory_caches_unknown_auth_certs(const or_options_t *options)
{
  return dir_server_mode(options) || options->BridgeRelay;
}