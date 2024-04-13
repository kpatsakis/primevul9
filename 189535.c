e_queue_transfer (GQueue *src_queue,
                  GQueue *dst_queue)
{
	g_return_if_fail (src_queue != NULL);
	g_return_if_fail (dst_queue != NULL);

	dst_queue->head = g_list_concat (dst_queue->head, src_queue->head);
	dst_queue->tail = g_list_last (dst_queue->head);
	dst_queue->length += src_queue->length;

	src_queue->head = NULL;
	src_queue->tail = NULL;
	src_queue->length = 0;
}