static int vmxnet3_mcast_list_pre_load(void *opaque)
{
    VMXNET3State *s = opaque;

    s->mcast_list = g_malloc(s->mcast_list_buff_size);

    return 0;
}