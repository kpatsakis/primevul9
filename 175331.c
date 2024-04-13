uint_t aubio_tempo_set_tatum_signature (aubio_tempo_t *o, uint_t signature) {
  if (signature < 1 || signature > 64) {
    return AUBIO_FAIL;
  } else {
    o->tatum_signature = signature;
    return AUBIO_OK;
  }
}