smpl_t aubio_tempo_get_period_s (aubio_tempo_t *o)
{
  return aubio_beattracking_get_period_s (o->bt);
}