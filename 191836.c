directory_permits_begindir_requests(const or_options_t *options)
{
  return options->BridgeRelay != 0 || dir_server_mode(options);
}