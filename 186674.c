static void mptsas_process_ioc_facts(MPTSASState *s,
                                     MPIMsgIOCFacts *req)
{
    MPIMsgIOCFactsReply reply;

    mptsas_fix_ioc_facts_endianness(req);

    QEMU_BUILD_BUG_ON(MPTSAS_MAX_REQUEST_SIZE < sizeof(*req));
    QEMU_BUILD_BUG_ON(sizeof(s->doorbell_msg) < sizeof(*req));
    QEMU_BUILD_BUG_ON(sizeof(s->doorbell_reply) < sizeof(reply));

    memset(&reply, 0, sizeof(reply));
    reply.MsgVersion                 = 0x0105;
    reply.MsgLength                  = sizeof(reply) / 4;
    reply.Function                   = req->Function;
    reply.MsgContext                 = req->MsgContext;
    reply.MaxChainDepth              = MPTSAS_MAXIMUM_CHAIN_DEPTH;
    reply.WhoInit                    = s->who_init;
    reply.BlockSize                  = MPTSAS_MAX_REQUEST_SIZE / sizeof(uint32_t);
    reply.ReplyQueueDepth            = ARRAY_SIZE(s->reply_post) - 1;
    QEMU_BUILD_BUG_ON(ARRAY_SIZE(s->reply_post) != ARRAY_SIZE(s->reply_free));

    reply.RequestFrameSize           = 128;
    reply.ProductID                  = MPTSAS1068_PRODUCT_ID;
    reply.CurrentHostMfaHighAddr     = s->host_mfa_high_addr >> 32;
    reply.GlobalCredits              = ARRAY_SIZE(s->request_post) - 1;
    reply.NumberOfPorts              = MPTSAS_NUM_PORTS;
    reply.CurrentSenseBufferHighAddr = s->sense_buffer_high_addr >> 32;
    reply.CurReplyFrameSize          = s->reply_frame_size;
    reply.MaxDevices                 = s->max_devices;
    reply.MaxBuses                   = s->max_buses;
    reply.FWVersionDev               = 0;
    reply.FWVersionUnit              = 0x92;
    reply.FWVersionMinor             = 0x32;
    reply.FWVersionMajor             = 0x1;

    mptsas_fix_ioc_facts_reply_endianness(&reply);
    mptsas_reply(s, (MPIDefaultReply *)&reply);
}