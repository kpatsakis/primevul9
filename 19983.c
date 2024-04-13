static AddrRange addrrange_make(Int128 start, Int128 size)
{
    return (AddrRange) { start, size };
}