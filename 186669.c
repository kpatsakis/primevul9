static void mptsas_set_fault(MPTSASState *s, uint32_t code)
{
    if ((s->state & MPI_IOC_STATE_FAULT) == 0) {
        s->state = MPI_IOC_STATE_FAULT | code;
    }
}