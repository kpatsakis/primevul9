directory_too_idle_to_fetch_descriptors(const or_options_t *options,
                                        time_t now)
{
  return !directory_caches_dir_info(options) &&
         !options->FetchUselessDescriptors &&
         rep_hist_circbuilding_dormant(now);
}