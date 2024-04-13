pixelTypeSize (PixelType type)
{
    int size;

    switch (type)
    {
      case OPENEXR_IMF_INTERNAL_NAMESPACE::UINT:
	
	size = Xdr::size <unsigned int> ();
	break;

      case OPENEXR_IMF_INTERNAL_NAMESPACE::HALF:

	size = Xdr::size <half> ();
	break;

      case OPENEXR_IMF_INTERNAL_NAMESPACE::FLOAT:

	size = Xdr::size <float> ();
	break;

      default:

	throw IEX_NAMESPACE::ArgExc ("Unknown pixel type.");
    }

    return size;
}