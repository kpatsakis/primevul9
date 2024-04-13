GopherStateData::swanSong()
{
    if (entry)
        entry->unlock("gopherState");

    if (buf) {
        memFree(buf, MEM_4K_BUF);
        buf = nullptr;
    }
}