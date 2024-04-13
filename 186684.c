static void mptsas_process_port_enable(MPTSASState *s,
                                       MPIMsgPortEnable *req)
{
    MPIMsgPortEnableReply reply;

    mptsas_fix_port_enable_endianness(req);

    QEMU_BUILD_BUG_ON(MPTSAS_MAX_REQUEST_SIZE < sizeof(*req));
    QEMU_BUILD_BUG_ON(sizeof(s->doorbell_msg) < sizeof(*req));
    QEMU_BUILD_BUG_ON(sizeof(s->doorbell_reply) < sizeof(reply));

    memset(&reply, 0, sizeof(reply));
    reply.MsgLength  = sizeof(reply) / 4;
    reply.PortNumber = req->PortNumber;
    reply.Function   = req->Function;
    reply.MsgContext = req->MsgContext;

    mptsas_fix_port_enable_reply_endianness(&reply);
    mptsas_reply(s, (MPIDefaultReply *)&reply);
}