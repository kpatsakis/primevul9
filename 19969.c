static bool addrrange_equal(AddrRange r1, AddrRange r2)
{
    return int128_eq(r1.start, r2.start) && int128_eq(r1.size, r2.size);
}