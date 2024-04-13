static int mptsas_post_load(void *opaque, int version_id)
{
    MPTSASState *s = opaque;

    if (s->doorbell_idx > s->doorbell_cnt ||
        s->doorbell_cnt > ARRAY_SIZE(s->doorbell_msg) ||
        s->doorbell_reply_idx > s->doorbell_reply_size ||
        s->doorbell_reply_size > ARRAY_SIZE(s->doorbell_reply) ||
        MPTSAS_FIFO_INVALID(s, request_post) ||
        MPTSAS_FIFO_INVALID(s, reply_post) ||
        MPTSAS_FIFO_INVALID(s, reply_free) ||
        s->diagnostic_idx > 4) {
        return -EINVAL;
    }

    return 0;
}