uint_t aubio_tempo_get_last (aubio_tempo_t *o)
{
  return o->last_beat + o->delay;
}