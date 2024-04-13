smpl_t aubio_tempo_get_confidence(aubio_tempo_t *o) {
  return aubio_beattracking_get_confidence(o->bt);
}