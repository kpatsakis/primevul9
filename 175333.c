aubio_tempo_t * new_aubio_tempo (const char_t * tempo_mode,
    uint_t buf_size, uint_t hop_size, uint_t samplerate)
{
  aubio_tempo_t * o = AUBIO_NEW(aubio_tempo_t);
  char_t specdesc_func[PATH_MAX];
  o->samplerate = samplerate;
  // check parameters are valid
  if ((sint_t)hop_size < 1) {
    AUBIO_ERR("tempo: got hop size %d, but can not be < 1\n", hop_size);
    goto beach;
  } else if ((sint_t)buf_size < 2) {
    AUBIO_ERR("tempo: got window size %d, but can not be < 2\n", buf_size);
    goto beach;
  } else if (buf_size < hop_size) {
    AUBIO_ERR("tempo: hop size (%d) is larger than window size (%d)\n", buf_size, hop_size);
    goto beach;
  } else if ((sint_t)samplerate < 1) {
    AUBIO_ERR("tempo: samplerate (%d) can not be < 1\n", samplerate);
    goto beach;
  }

  /* length of observations, worth about 6 seconds */
  o->winlen = aubio_next_power_of_two(5.8 * samplerate / hop_size);
  if (o->winlen < 4) o->winlen = 4;
  o->step = o->winlen/4;
  o->blockpos = 0;
  o->threshold = 0.3;
  o->silence = -90.;
  o->total_frames = 0;
  o->last_beat = 0;
  o->delay = 0;
  o->hop_size = hop_size;
  o->dfframe  = new_fvec(o->winlen);
  o->fftgrain = new_cvec(buf_size);
  o->out      = new_fvec(o->step);
  o->pv       = new_aubio_pvoc(buf_size, hop_size);
  o->pp       = new_aubio_peakpicker();
  aubio_peakpicker_set_threshold (o->pp, o->threshold);
  if ( strcmp(tempo_mode, "default") == 0 ) {
    strncpy(specdesc_func, "specflux", PATH_MAX - 1);
  } else {
    strncpy(specdesc_func, tempo_mode, PATH_MAX - 1);
    specdesc_func[PATH_MAX - 1] = '\0';
  }
  o->od       = new_aubio_specdesc(specdesc_func,buf_size);
  o->of       = new_fvec(1);
  o->bt       = new_aubio_beattracking(o->winlen, o->hop_size, o->samplerate);
  o->onset    = new_fvec(1);
  /*if (usedoubled)    {
    o2 = new_aubio_specdesc(type_onset2,buffer_size);
    onset2 = new_fvec(1);
  }*/
  o->last_tatum = 0;
  o->tatum_signature = 4;
  return o;

beach:
  AUBIO_FREE(o);
  return NULL;
}