void usbredirparser_caps_set_cap(uint32_t *caps, int cap)
{
    caps[cap / 32] |= 1 << (cap % 32);
}