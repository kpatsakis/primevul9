smpl_t aubio_tempo_get_bpm(aubio_tempo_t *o) {
  return aubio_beattracking_get_bpm(o->bt);
}