void * TDStretch::operator new(size_t s)
{
    // Notice! don't use "new TDStretch" directly, use "newInstance" to create a new instance instead!
    ST_THROW_RT_ERROR("Error in TDStretch::new: Don't use 'new TDStretch' directly, use 'newInstance' member instead!");
    return newInstance();
}