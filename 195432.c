static INLINE BOOL freerdp_bitmap_planar_delta_encode_planes(BYTE* inPlanes[4], UINT32 width,
                                                             UINT32 height, BYTE* outPlanes[4])
{
	UINT32 i;

	for (i = 0; i < 4; i++)
	{
		outPlanes[i] =
		    freerdp_bitmap_planar_delta_encode_plane(inPlanes[i], width, height, outPlanes[i]);

		if (!outPlanes[i])
			return FALSE;
	}

	return TRUE;
}