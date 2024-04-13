static void mptsas_process_event_notification(MPTSASState *s,
                                              MPIMsgEventNotify *req)
{
    MPIMsgEventNotifyReply reply;

    mptsas_fix_event_notification_endianness(req);

    QEMU_BUILD_BUG_ON(MPTSAS_MAX_REQUEST_SIZE < sizeof(*req));
    QEMU_BUILD_BUG_ON(sizeof(s->doorbell_msg) < sizeof(*req));
    QEMU_BUILD_BUG_ON(sizeof(s->doorbell_reply) < sizeof(reply));

    /* Don't even bother storing whether event notification is enabled,
     * since it is not accessible.
     */

    memset(&reply, 0, sizeof(reply));
    reply.EventDataLength = sizeof(reply.Data) / 4;
    reply.MsgLength       = sizeof(reply) / 4;
    reply.Function        = req->Function;

    /* This is set because events are sent through the reply FIFOs.  */
    reply.MsgFlags        = MPI_MSGFLAGS_CONTINUATION_REPLY;

    reply.MsgContext      = req->MsgContext;
    reply.Event           = MPI_EVENT_EVENT_CHANGE;
    reply.Data[0]         = !!req->Switch;

    mptsas_fix_event_notification_reply_endianness(&reply);
    mptsas_reply(s, (MPIDefaultReply *)&reply);
}