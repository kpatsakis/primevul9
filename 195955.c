NetQueue *qemu_new_net_queue(NetQueueDeliverFunc *deliver, void *opaque)
{
    NetQueue *queue;

    queue = g_new0(NetQueue, 1);

    queue->opaque = opaque;
    queue->nq_maxlen = 10000;
    queue->nq_count = 0;
    queue->deliver = deliver;

    QTAILQ_INIT(&queue->packets);

    queue->delivering = 0;

    return queue;
}