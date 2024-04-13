dirserv_set_routerstatus_testing(routerstatus_t *rs)
{
  const or_options_t *options = get_options();

  tor_assert(options->TestingTorNetwork);

  if (routerset_contains_routerstatus(options->TestingDirAuthVoteExit,
                                      rs, 0)) {
    rs->is_exit = 1;
  } else if (options->TestingDirAuthVoteExitIsStrict) {
    rs->is_exit = 0;
  }

  if (routerset_contains_routerstatus(options->TestingDirAuthVoteGuard,
                                      rs, 0)) {
    rs->is_possible_guard = 1;
  } else if (options->TestingDirAuthVoteGuardIsStrict) {
    rs->is_possible_guard = 0;
  }

  if (routerset_contains_routerstatus(options->TestingDirAuthVoteHSDir,
                                      rs, 0)) {
    rs->is_hs_dir = 1;
  } else if (options->TestingDirAuthVoteHSDirIsStrict) {
    rs->is_hs_dir = 0;
  }
}