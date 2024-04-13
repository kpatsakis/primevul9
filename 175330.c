uint_t aubio_tempo_set_silence(aubio_tempo_t * o, smpl_t silence) {
  o->silence = silence;
  return AUBIO_OK;
}