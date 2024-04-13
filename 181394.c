static uint32_t qpf_hash(QpfEntry e)
{
    return qemu_xxhash7(e.ino, e.dev, 0, 0, 0);
}