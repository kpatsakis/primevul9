aubio_filterbank_set_norm (aubio_filterbank_t *f, smpl_t norm)
{
  if (norm != 0 && norm != 1) return AUBIO_FAIL;
  f->norm = norm;
  return AUBIO_OK;
}