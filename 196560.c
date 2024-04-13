register_pk_elg_progress ( void (*cb)( void *, int), void *cb_data )
{
    progress_cb = cb;
    progress_cb_data = cb_data;
}