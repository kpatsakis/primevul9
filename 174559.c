lips_rle_encode(byte * inBuff, byte * outBuff, int Length)
{
    int i = 0;
    byte value;
    int count = 0;
    byte *ptr = inBuff;

    value = *ptr;
    ptr++;

    while (ptr < inBuff + Length) {
        if (*ptr == value) {
            count++;
            if (count > RLECOUNTMAX) {
                *outBuff++ = RLECOUNTMAX;
                *outBuff++ = value;
                i += 2;
                count = 0;
            }
        } else {
            *outBuff++ = count;
            *outBuff++ = value;
            i += 2;
            count = 0;
            value = *ptr;
        }
        ptr++;
    }
    *outBuff++ = count;
    *outBuff++ = value;
    i += 2;

    return i;
}