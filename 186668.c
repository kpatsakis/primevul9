static int mptsas_hard_reset(MPTSASState *s)
{
    mptsas_soft_reset(s);

    s->intr_mask = MPI_HIM_DIM | MPI_HIM_RIM;

    s->host_mfa_high_addr = 0;
    s->sense_buffer_high_addr = 0;
    s->reply_frame_size = 0;
    s->max_devices = MPTSAS_NUM_PORTS;
    s->max_buses = 1;

    return 0;
}