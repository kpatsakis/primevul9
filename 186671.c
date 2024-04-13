static void mptsas_command_complete(SCSIRequest *sreq,
        size_t resid)
{
    MPTSASRequest *req = sreq->hba_private;
    MPTSASState *s = req->dev;
    uint8_t sense_buf[SCSI_SENSE_BUF_SIZE];
    uint8_t sense_len;

    hwaddr sense_buffer_addr = req->dev->sense_buffer_high_addr |
            req->scsi_io.SenseBufferLowAddr;

    trace_mptsas_command_complete(s, req->scsi_io.MsgContext,
                                  sreq->status, resid);

    sense_len = scsi_req_get_sense(sreq, sense_buf, SCSI_SENSE_BUF_SIZE);
    if (sense_len > 0) {
        pci_dma_write(PCI_DEVICE(s), sense_buffer_addr, sense_buf,
                      MIN(req->scsi_io.SenseBufferLength, sense_len));
    }

    if (sreq->status != GOOD || resid ||
        req->dev->doorbell_state == DOORBELL_WRITE) {
        MPIMsgSCSIIOReply reply;

        memset(&reply, 0, sizeof(reply));
        reply.TargetID          = req->scsi_io.TargetID;
        reply.Bus               = req->scsi_io.Bus;
        reply.MsgLength         = sizeof(reply) / 4;
        reply.Function          = req->scsi_io.Function;
        reply.CDBLength         = req->scsi_io.CDBLength;
        reply.SenseBufferLength = req->scsi_io.SenseBufferLength;
        reply.MsgFlags          = req->scsi_io.MsgFlags;
        reply.MsgContext        = req->scsi_io.MsgContext;
        reply.SCSIStatus        = sreq->status;
        if (sreq->status == GOOD) {
            reply.TransferCount = req->scsi_io.DataLength - resid;
            if (resid) {
                reply.IOCStatus     = MPI_IOCSTATUS_SCSI_DATA_UNDERRUN;
            }
        } else {
            reply.SCSIState     = MPI_SCSI_STATE_AUTOSENSE_VALID;
            reply.SenseCount    = sense_len;
            reply.IOCStatus     = MPI_IOCSTATUS_SCSI_DATA_UNDERRUN;
        }

        mptsas_fix_scsi_io_reply_endianness(&reply);
        mptsas_post_reply(req->dev, (MPIDefaultReply *)&reply);
    } else {
        mptsas_turbo_reply(req->dev, req->scsi_io.MsgContext);
    }

    mptsas_free_request(req);
}