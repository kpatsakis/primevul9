static int rtl8139_RxWrap(RTL8139State *s)
{
    /* wrapping enabled; assume 1.5k more buffer space if size < 65536 */
    return (s->RxConfig & (1 << 7));
}