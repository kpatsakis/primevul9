smpl_t aubio_tempo_get_last_tatum (aubio_tempo_t *o) {
  return (smpl_t)o->last_tatum - o->delay;
}