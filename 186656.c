static void mptsas_process_ioc_init(MPTSASState *s, MPIMsgIOCInit *req)
{
    MPIMsgIOCInitReply reply;

    mptsas_fix_ioc_init_endianness(req);

    QEMU_BUILD_BUG_ON(MPTSAS_MAX_REQUEST_SIZE < sizeof(*req));
    QEMU_BUILD_BUG_ON(sizeof(s->doorbell_msg) < sizeof(*req));
    QEMU_BUILD_BUG_ON(sizeof(s->doorbell_reply) < sizeof(reply));

    s->who_init               = req->WhoInit;
    s->reply_frame_size       = req->ReplyFrameSize;
    s->max_buses              = req->MaxBuses;
    s->max_devices            = req->MaxDevices ? req->MaxDevices : 256;
    s->host_mfa_high_addr     = (hwaddr)req->HostMfaHighAddr << 32;
    s->sense_buffer_high_addr = (hwaddr)req->SenseBufferHighAddr << 32;

    if (s->state == MPI_IOC_STATE_READY) {
        s->state = MPI_IOC_STATE_OPERATIONAL;
    }

    memset(&reply, 0, sizeof(reply));
    reply.WhoInit    = s->who_init;
    reply.MsgLength  = sizeof(reply) / 4;
    reply.Function   = req->Function;
    reply.MaxDevices = s->max_devices;
    reply.MaxBuses   = s->max_buses;
    reply.MsgContext = req->MsgContext;

    mptsas_fix_ioc_init_reply_endianness(&reply);
    mptsas_reply(s, (MPIDefaultReply *)&reply);
}