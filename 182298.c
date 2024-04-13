static int vmxnet3_pre_save(void *opaque)
{
    VMXNET3State *s = opaque;

    s->mcast_list_buff_size = s->mcast_list_len * sizeof(MACAddr);

    return 0;
}