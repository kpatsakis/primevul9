directory_fetches_dir_info_later(const or_options_t *options)
{
  return options->UseBridges != 0;
}