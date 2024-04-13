policies_parse_from_options(or_options_t *options)
{
  int ret = 0;
  if (load_policy_from_option(options->SocksPolicy, &socks_policy, -1) < 0)
    ret = -1;
  if (load_policy_from_option(options->DirPolicy, &dir_policy, -1) < 0)
    ret = -1;
  if (load_policy_from_option(options->AuthDirReject,
                              &authdir_reject_policy, ADDR_POLICY_REJECT) < 0)
    ret = -1;
  if (load_policy_from_option(options->AuthDirInvalid,
                              &authdir_invalid_policy, ADDR_POLICY_REJECT) < 0)
    ret = -1;
  if (load_policy_from_option(options->AuthDirBadDir,
                              &authdir_baddir_policy, ADDR_POLICY_REJECT) < 0)
    ret = -1;
  if (load_policy_from_option(options->AuthDirBadExit,
                              &authdir_badexit_policy, ADDR_POLICY_REJECT) < 0)
    ret = -1;
  if (parse_reachable_addresses() < 0)
    ret = -1;
  return ret;
}