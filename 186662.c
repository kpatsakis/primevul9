static void mptsas_process_port_facts(MPTSASState *s,
                                     MPIMsgPortFacts *req)
{
    MPIMsgPortFactsReply reply;

    mptsas_fix_port_facts_endianness(req);

    QEMU_BUILD_BUG_ON(MPTSAS_MAX_REQUEST_SIZE < sizeof(*req));
    QEMU_BUILD_BUG_ON(sizeof(s->doorbell_msg) < sizeof(*req));
    QEMU_BUILD_BUG_ON(sizeof(s->doorbell_reply) < sizeof(reply));

    memset(&reply, 0, sizeof(reply));
    reply.MsgLength  = sizeof(reply) / 4;
    reply.Function   = req->Function;
    reply.PortNumber = req->PortNumber;
    reply.MsgContext = req->MsgContext;

    if (req->PortNumber < MPTSAS_NUM_PORTS) {
        reply.PortType      = MPI_PORTFACTS_PORTTYPE_SAS;
        reply.MaxDevices    = MPTSAS_NUM_PORTS;
        reply.PortSCSIID    = MPTSAS_NUM_PORTS;
        reply.ProtocolFlags = MPI_PORTFACTS_PROTOCOL_LOGBUSADDR | MPI_PORTFACTS_PROTOCOL_INITIATOR;
    }

    mptsas_fix_port_facts_reply_endianness(&reply);
    mptsas_reply(s, (MPIDefaultReply *)&reply);
}