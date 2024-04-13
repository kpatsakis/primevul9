static uint32_t mptsas_doorbell_read(MPTSASState *s)
{
    uint32_t ret;

    ret = (s->who_init << MPI_DOORBELL_WHO_INIT_SHIFT) & MPI_DOORBELL_WHO_INIT_MASK;
    ret |= s->state;
    switch (s->doorbell_state) {
    case DOORBELL_NONE:
        break;

    case DOORBELL_WRITE:
        ret |= MPI_DOORBELL_ACTIVE;
        break;

    case DOORBELL_READ:
        /* Get rid of the IOC fault code.  */
        ret &= ~MPI_DOORBELL_DATA_MASK;

        assert(s->intr_status & MPI_HIS_DOORBELL_INTERRUPT);
        assert(s->doorbell_reply_idx <= s->doorbell_reply_size);

        ret |= MPI_DOORBELL_ACTIVE;
        if (s->doorbell_reply_idx < s->doorbell_reply_size) {
            /* For more information about this endian switch, see the
             * commit message for commit 36b62ae ("fw_cfg: fix endianness in
             * fw_cfg_data_mem_read() / _write()", 2015-01-16).
             */
            ret |= le16_to_cpu(s->doorbell_reply[s->doorbell_reply_idx++]);
        }
        break;

    default:
        abort();
    }

    return ret;
}