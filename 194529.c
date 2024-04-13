j2k_read(void *p_buffer, OPJ_SIZE_T p_nb_bytes, void *p_user_data) {
    ImagingCodecState state = (ImagingCodecState)p_user_data;

    size_t len = _imaging_read_pyFd(state->fd, p_buffer, p_nb_bytes);

    return len ? len : (OPJ_SIZE_T)-1;
}