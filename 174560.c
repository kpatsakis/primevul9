GetNumSameData(const byte * curPtr, const int maxnum)
{
    int count = 1;

    if (1 == maxnum) {
        return (1);
    }
    while (maxnum > count && *curPtr == *(curPtr + count)) {
        count++;
    }

    return (count);
}