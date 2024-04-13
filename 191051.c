static bool is_buffer_zero(unsigned char *buf,int buflen)
{
    if(buflen >= (int)sizeof(long))
    {
        // align to word boundary
        buflen -= (intptr_t)buf % sizeof(long);

        while((intptr_t)buf % sizeof(long))
        {
            if(*buf++)
                return false;
        }

        // read in words
        long *ptr = (long*)buf;
        buf += buflen - buflen % sizeof(long);
        buflen %= sizeof(long);

        while(ptr < (long*)buf)
        {
            if(*ptr++)
                return false;
        }
    }

    while(buflen--)
    {
        if(*buf++)
            return false;
    }

    return true;
}