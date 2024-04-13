parse_reachable_addresses(void)
{
  or_options_t *options = get_options();
  int ret = 0;

  if (options->ReachableDirAddresses &&
      options->ReachableORAddresses &&
      options->ReachableAddresses) {
    log_warn(LD_CONFIG,
             "Both ReachableDirAddresses and ReachableORAddresses are set. "
             "ReachableAddresses setting will be ignored.");
  }
  addr_policy_list_free(reachable_or_addr_policy);
  reachable_or_addr_policy = NULL;
  if (!options->ReachableORAddresses && options->ReachableAddresses)
    log_info(LD_CONFIG,
             "Using ReachableAddresses as ReachableORAddresses.");
  if (parse_addr_policy(options->ReachableORAddresses ?
                          options->ReachableORAddresses :
                          options->ReachableAddresses,
                        &reachable_or_addr_policy, ADDR_POLICY_ACCEPT)) {
    log_warn(LD_CONFIG,
             "Error parsing Reachable%sAddresses entry; ignoring.",
             options->ReachableORAddresses ? "OR" : "");
    ret = -1;
  }

  addr_policy_list_free(reachable_dir_addr_policy);
  reachable_dir_addr_policy = NULL;
  if (!options->ReachableDirAddresses && options->ReachableAddresses)
    log_info(LD_CONFIG,
             "Using ReachableAddresses as ReachableDirAddresses");
  if (parse_addr_policy(options->ReachableDirAddresses ?
                          options->ReachableDirAddresses :
                          options->ReachableAddresses,
                        &reachable_dir_addr_policy, ADDR_POLICY_ACCEPT)) {
    if (options->ReachableDirAddresses)
      log_warn(LD_CONFIG,
               "Error parsing ReachableDirAddresses entry; ignoring.");
    ret = -1;
  }
  return ret;
}