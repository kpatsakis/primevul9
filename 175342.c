uint_t aubio_tempo_was_tatum (aubio_tempo_t *o)
{
  uint_t last_tatum_distance = o->total_frames - o->last_tatum;
  smpl_t beat_period = aubio_tempo_get_period(o);
  smpl_t tatum_period = beat_period / o->tatum_signature;
  if (last_tatum_distance < o->hop_size) {
    o->last_tatum = o->last_beat;
    return 2;
  }
  else if (last_tatum_distance > tatum_period) {
    if ( last_tatum_distance + o->hop_size > beat_period ) {
      // next beat is too close, pass
      return 0;
    }
    o->last_tatum = o->total_frames;
    return 1;
  }
  return 0;
}