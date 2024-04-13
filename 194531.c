j2k_skip(OPJ_OFF_T p_nb_bytes, void *p_user_data) {
    off_t pos;
    ImagingCodecState state = (ImagingCodecState)p_user_data;

    _imaging_seek_pyFd(state->fd, p_nb_bytes, SEEK_CUR);
    pos = _imaging_tell_pyFd(state->fd);

    return pos ? pos : (OPJ_OFF_T)-1;
}