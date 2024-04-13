TransposerBase *TransposerBase::newInstance()
{
#ifdef SOUNDTOUCH_INTEGER_SAMPLES
    // Notice: For integer arithmetics support only linear algorithm (due to simplest calculus)
    return ::new InterpolateLinearInteger;
#else
    switch (algorithm)
    {
        case LINEAR:
            return new InterpolateLinearFloat;

        case CUBIC:
            return new InterpolateCubic;

        case SHANNON:
            return new InterpolateShannon;

        default:
            assert(false);
            return NULL;
    }
#endif
}