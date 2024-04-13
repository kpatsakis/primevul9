smpl_t aubio_tempo_get_period (aubio_tempo_t *o)
{
  return aubio_beattracking_get_period (o->bt);
}