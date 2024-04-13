aubio_filterbank_set_power (aubio_filterbank_t *f, smpl_t power)
{
  f->power = power;
  return AUBIO_OK;
}