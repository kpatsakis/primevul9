vmxnet3_dump_virt_hdr(struct virtio_net_hdr *vhdr)
{
    VMW_PKPRN("VHDR: flags 0x%x, gso_type: 0x%x, hdr_len: %d, gso_size: %d, "
              "csum_start: %d, csum_offset: %d",
              vhdr->flags, vhdr->gso_type, vhdr->hdr_len, vhdr->gso_size,
              vhdr->csum_start, vhdr->csum_offset);
}