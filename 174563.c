lips_mode3format_encode(byte * inBuff, byte * outBuff, int Length)
{
    int size = 0;

    while (Length) {
        int count;

        if (1 < (count = GetNumSameData(inBuff,
                                        Length > 257 ? 257 : Length))) {
            Length -= count;
            size += 3;

            *outBuff++ = *inBuff;
            *outBuff++ = *inBuff;
            *outBuff++ = count - 2;
            inBuff += count;
        } else {
            count = GetNumWrongData(inBuff, Length);
            Length -= count;
            size += count;

            while (count--) {
                *outBuff++ = *inBuff++;
            }
        }
    }

    return (size);
}