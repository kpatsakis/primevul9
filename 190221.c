validate_addr_policies(or_options_t *options, char **msg)
{
  /* XXXX Maybe merge this into parse_policies_from_options, to make sure
   * that the two can't go out of sync. */

  smartlist_t *addr_policy=NULL;
  *msg = NULL;

  if (policies_parse_exit_policy(options->ExitPolicy, &addr_policy,
                                 options->ExitPolicyRejectPrivate, NULL,
                                 !options->BridgeRelay))
    REJECT("Error in ExitPolicy entry.");

  /* The rest of these calls *append* to addr_policy. So don't actually
   * use the results for anything other than checking if they parse! */
  if (parse_addr_policy(options->DirPolicy, &addr_policy, -1))
    REJECT("Error in DirPolicy entry.");
  if (parse_addr_policy(options->SocksPolicy, &addr_policy, -1))
    REJECT("Error in SocksPolicy entry.");
  if (parse_addr_policy(options->AuthDirReject, &addr_policy,
                        ADDR_POLICY_REJECT))
    REJECT("Error in AuthDirReject entry.");
  if (parse_addr_policy(options->AuthDirInvalid, &addr_policy,
                        ADDR_POLICY_REJECT))
    REJECT("Error in AuthDirInvalid entry.");
  if (parse_addr_policy(options->AuthDirBadDir, &addr_policy,
                        ADDR_POLICY_REJECT))
    REJECT("Error in AuthDirBadDir entry.");
  if (parse_addr_policy(options->AuthDirBadExit, &addr_policy,
                        ADDR_POLICY_REJECT))
    REJECT("Error in AuthDirBadExit entry.");

  if (parse_addr_policy(options->ReachableAddresses, &addr_policy,
                        ADDR_POLICY_ACCEPT))
    REJECT("Error in ReachableAddresses entry.");
  if (parse_addr_policy(options->ReachableORAddresses, &addr_policy,
                        ADDR_POLICY_ACCEPT))
    REJECT("Error in ReachableORAddresses entry.");
  if (parse_addr_policy(options->ReachableDirAddresses, &addr_policy,
                        ADDR_POLICY_ACCEPT))
    REJECT("Error in ReachableDirAddresses entry.");

 err:
  addr_policy_list_free(addr_policy);
  return *msg ? -1 : 0;
#undef REJECT
}