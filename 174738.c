pwg_free_finishings(
    _pwg_finishings_t *f)		/* I - Finishings value */
{
  cupsFreeOptions(f->num_options, f->options);
  free(f);
}