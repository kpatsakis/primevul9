gs_abort(const gs_memory_t *mem)
{
    /* In previous versions, we tried to do a cleanup (using gs_to_exit),
     * but more often than not, that will trip another abort and create
     * an infinite recursion. So just abort without trying to cleanup.
     */
    gp_do_exit(1);
}