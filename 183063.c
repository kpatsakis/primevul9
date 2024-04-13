void EmitIntent(cmsIOHANDLER* m, cmsUInt32Number RenderingIntent)
{
    const char *intent;

    switch (RenderingIntent) {

        case INTENT_PERCEPTUAL:            intent = "Perceptual"; break;
        case INTENT_RELATIVE_COLORIMETRIC: intent = "RelativeColorimetric"; break;
        case INTENT_ABSOLUTE_COLORIMETRIC: intent = "AbsoluteColorimetric"; break;
        case INTENT_SATURATION:            intent = "Saturation"; break;

        default: intent = "Undefined"; break;
    }

    _cmsIOPrintf(m, "/RenderingIntent (%s)\n", intent );
}