free_cached_dir_(void *_d)
{
  cached_dir_t *d;
  if (!_d)
    return;

  d = (cached_dir_t *)_d;
  cached_dir_decref(d);
}