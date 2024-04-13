smpl_t aubio_tempo_get_last_s (aubio_tempo_t *o)
{
  return aubio_tempo_get_last (o) / (smpl_t) (o->samplerate);
}