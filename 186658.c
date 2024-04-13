static void mptsas_request_cancelled(SCSIRequest *sreq)
{
    MPTSASRequest *req = sreq->hba_private;
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
    reply.SCSIState         = MPI_SCSI_STATE_NO_SCSI_STATUS;
    reply.IOCStatus         = MPI_IOCSTATUS_SCSI_TASK_TERMINATED;

    mptsas_fix_scsi_io_reply_endianness(&reply);
    mptsas_post_reply(req->dev, (MPIDefaultReply *)&reply);
    mptsas_free_request(req);
}