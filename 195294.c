Mat_GetLibraryVersion(int *major,int *minor,int *release)
{
    if ( NULL != major )
        *major = MATIO_MAJOR_VERSION;
    if ( NULL != minor )
        *minor = MATIO_MINOR_VERSION;
    if ( NULL != release )
        *release = MATIO_RELEASE_LEVEL;
}