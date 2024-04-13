lips_packbits_encode(byte * inBuff, byte * outBuff, int Length)
{
    int size = 0;

    while (Length) {
        int count;

        if (1 < (count = GetNumSameData(inBuff,
                                        Length > 128 ? 128 : Length))) {
            Length -= count;
            size += 2;

            *outBuff++ = -(count - 1);
            *outBuff++ = *inBuff;
            inBuff += count;
        } else {
            count = GetNumWrongData(inBuff, Length > 128 ? 128 : Length);
            Length -= count;
            size += count + 1;

            *outBuff++ = count - 1;
            while (count--) {
                *outBuff++ = *inBuff++;
            }
        }
    }

    return (size);
}