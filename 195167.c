static unsigned int getKeyid(pgpDigParams sigp)
{
    return (sigp != NULL) ? pgpGrab(sigp->signid+4, 4) : 0;
}