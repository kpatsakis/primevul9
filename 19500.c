static UINT video_read_tsmm_presentation_req(VideoClientContext* context, wStream* s)
{
	TSMM_PRESENTATION_REQUEST req;

	if (Stream_GetRemainingLength(s) < 60)
	{
		WLog_ERR(TAG, "not enough bytes for a TSMM_PRESENTATION_REQUEST");
		return ERROR_INVALID_DATA;
	}

	Stream_Read_UINT8(s, req.PresentationId);
	Stream_Read_UINT8(s, req.Version);
	Stream_Read_UINT8(s, req.Command);
	Stream_Read_UINT8(s, req.FrameRate); /* FrameRate - reserved and ignored */

	Stream_Seek_UINT16(s); /* AverageBitrateKbps reserved and ignored */
	Stream_Seek_UINT16(s); /* reserved */

	Stream_Read_UINT32(s, req.SourceWidth);
	Stream_Read_UINT32(s, req.SourceHeight);
	Stream_Read_UINT32(s, req.ScaledWidth);
	Stream_Read_UINT32(s, req.ScaledHeight);
	Stream_Read_UINT64(s, req.hnsTimestampOffset);
	Stream_Read_UINT64(s, req.GeometryMappingId);
	Stream_Read(s, req.VideoSubtypeId, 16);

	Stream_Read_UINT32(s, req.cbExtra);

	if (Stream_GetRemainingLength(s) < req.cbExtra)
	{
		WLog_ERR(TAG, "not enough bytes for cbExtra of TSMM_PRESENTATION_REQUEST");
		return ERROR_INVALID_DATA;
	}

	req.pExtraData = Stream_Pointer(s);

	WLog_DBG(TAG,
	         "presentationReq: id:%" PRIu8 " version:%" PRIu8
	         " command:%s srcWidth/srcHeight=%" PRIu32 "x%" PRIu32 " scaled Width/Height=%" PRIu32
	         "x%" PRIu32 " timestamp=%" PRIu64 " mappingId=%" PRIx64 "",
	         req.PresentationId, req.Version, video_command_name(req.Command), req.SourceWidth,
	         req.SourceHeight, req.ScaledWidth, req.ScaledHeight, req.hnsTimestampOffset,
	         req.GeometryMappingId);

	return video_PresentationRequest(context, &req);
}