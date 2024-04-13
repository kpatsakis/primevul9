uint bjc_rand(gx_device_bjc_printer *dev)
{
    uint ret = (bjc_rand_seed[dev->bjc_j++] += bjc_rand_seed[dev->bjc_k++]);
    if(dev->bjc_j==55) dev->bjc_j = 0;
    if(dev->bjc_k==55) dev->bjc_k = 0;
    return ret & 0x03ff;
}                                             /* random numbers 0-1023 */