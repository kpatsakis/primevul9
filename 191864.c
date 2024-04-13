dirserv_get_flag_thresholds_line(void)
{
  char *result=NULL;
  const int measured_threshold =
    get_options()->MinMeasuredBWsForAuthToIgnoreAdvertised;
  const int enough_measured_bw = routers_with_measured_bw > measured_threshold;

  tor_asprintf(&result,
      "stable-uptime=%lu stable-mtbf=%lu "
      "fast-speed=%lu "
      "guard-wfu=%.03f%% guard-tk=%lu "
      "guard-bw-inc-exits=%lu guard-bw-exc-exits=%lu "
      "enough-mtbf=%d ignoring-advertised-bws=%d",
      (unsigned long)stable_uptime,
      (unsigned long)stable_mtbf,
      (unsigned long)fast_bandwidth_kb*1000,
      guard_wfu*100,
      (unsigned long)guard_tk,
      (unsigned long)guard_bandwidth_including_exits_kb*1000,
      (unsigned long)guard_bandwidth_excluding_exits_kb*1000,
      enough_mtbf_info ? 1 : 0,
      enough_measured_bw ? 1 : 0);

  return result;
}