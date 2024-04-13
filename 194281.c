del_aubio_filterbank (aubio_filterbank_t * fb)
{
  del_fmat (fb->filters);
  AUBIO_FREE (fb);
}