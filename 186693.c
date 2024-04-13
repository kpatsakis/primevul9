static void mptsas_doorbell_write(MPTSASState *s, uint32_t val)
{
    if (s->doorbell_state == DOORBELL_WRITE) {
        if (s->doorbell_idx < s->doorbell_cnt) {
            /* For more information about this endian switch, see the
             * commit message for commit 36b62ae ("fw_cfg: fix endianness in
             * fw_cfg_data_mem_read() / _write()", 2015-01-16).
             */
            s->doorbell_msg[s->doorbell_idx++] = cpu_to_le32(val);
            if (s->doorbell_idx == s->doorbell_cnt) {
                mptsas_process_message(s, (MPIRequestHeader *)s->doorbell_msg);
            }
        }
        return;
    }

    switch ((val & MPI_DOORBELL_FUNCTION_MASK) >> MPI_DOORBELL_FUNCTION_SHIFT) {
    case MPI_FUNCTION_IOC_MESSAGE_UNIT_RESET:
        mptsas_soft_reset(s);
        break;
    case MPI_FUNCTION_IO_UNIT_RESET:
        break;
    case MPI_FUNCTION_HANDSHAKE:
        s->doorbell_state = DOORBELL_WRITE;
        s->doorbell_idx = 0;
        s->doorbell_cnt = (val & MPI_DOORBELL_ADD_DWORDS_MASK)
            >> MPI_DOORBELL_ADD_DWORDS_SHIFT;
        s->intr_status |= MPI_HIS_DOORBELL_INTERRUPT;
        mptsas_update_interrupt(s);
        break;
    default:
        trace_mptsas_unhandled_doorbell_cmd(s, val);
        break;
    }
}