OPJ_BOOL opj_j2k_set_threads(opj_j2k_t *j2k, OPJ_UINT32 num_threads)
{
    if (opj_has_thread_support()) {
        opj_thread_pool_destroy(j2k->m_tp);
        j2k->m_tp = NULL;
        if (num_threads <= (OPJ_UINT32)INT_MAX) {
            j2k->m_tp = opj_thread_pool_create((int)num_threads);
        }
        if (j2k->m_tp == NULL) {
            j2k->m_tp = opj_thread_pool_create(0);
            return OPJ_FALSE;
        }
        return OPJ_TRUE;
    }
    return OPJ_FALSE;
}