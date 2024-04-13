numSamples (int s, int a, int b)
{
    int a1 = divp (a, s);
    int b1 = divp (b, s);
    return  b1 - a1 + ((a1 * s < a)? 0: 1);
}