smpl_t aubio_tempo_get_last_ms (aubio_tempo_t *o)
{
  return aubio_tempo_get_last_s (o) * 1000.;
}