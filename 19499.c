static UINT video_PresentationRequest(VideoClientContext* video, TSMM_PRESENTATION_REQUEST* req)
{
	VideoClientContextPriv* priv = video->priv;
	PresentationContext* presentation;
	UINT ret = CHANNEL_RC_OK;

	presentation = priv->currentPresentation;

	if (req->Command == TSMM_START_PRESENTATION)
	{
		MAPPED_GEOMETRY* geom;
		TSMM_PRESENTATION_RESPONSE resp;

		if (memcmp(req->VideoSubtypeId, MFVideoFormat_H264, 16) != 0)
		{
			WLog_ERR(TAG, "not a H264 video, ignoring request");
			return CHANNEL_RC_OK;
		}

		if (presentation)
		{
			if (presentation->PresentationId == req->PresentationId)
			{
				WLog_ERR(TAG, "ignoring start request for existing presentation %d",
				         req->PresentationId);
				return CHANNEL_RC_OK;
			}

			WLog_ERR(TAG, "releasing current presentation %d", req->PresentationId);
			PresentationContext_unref(presentation);
			presentation = priv->currentPresentation = NULL;
		}

		if (!priv->geometry)
		{
			WLog_ERR(TAG, "geometry channel not ready, ignoring request");
			return CHANNEL_RC_OK;
		}

		geom = HashTable_GetItemValue(priv->geometry->geometries, &(req->GeometryMappingId));
		if (!geom)
		{
			WLog_ERR(TAG, "geometry mapping 0x%" PRIx64 " not registered", req->GeometryMappingId);
			return CHANNEL_RC_OK;
		}

		WLog_DBG(TAG, "creating presentation 0x%x", req->PresentationId);
		presentation = PresentationContext_new(
		    video, req->PresentationId, geom->topLevelLeft + geom->left,
		    geom->topLevelTop + geom->top, req->SourceWidth, req->SourceHeight);
		if (!presentation)
		{
			WLog_ERR(TAG, "unable to create presentation video");
			return CHANNEL_RC_NO_MEMORY;
		}

		mappedGeometryRef(geom);
		presentation->geometry = geom;

		priv->currentPresentation = presentation;
		presentation->video = video;
		presentation->SourceWidth = req->SourceWidth;
		presentation->SourceHeight = req->SourceHeight;
		presentation->ScaledWidth = req->ScaledWidth;
		presentation->ScaledHeight = req->ScaledHeight;

		geom->custom = presentation;
		geom->MappedGeometryUpdate = video_onMappedGeometryUpdate;
		geom->MappedGeometryClear = video_onMappedGeometryClear;

		/* send back response */
		resp.PresentationId = req->PresentationId;
		ret = video_control_send_presentation_response(video, &resp);
	}
	else if (req->Command == TSMM_STOP_PRESENTATION)
	{
		WLog_DBG(TAG, "stopping presentation 0x%x", req->PresentationId);
		if (!presentation)
		{
			WLog_ERR(TAG, "unknown presentation to stop %d", req->PresentationId);
			return CHANNEL_RC_OK;
		}

		priv->currentPresentation = NULL;
		priv->droppedFrames = 0;
		priv->publishedFrames = 0;
		PresentationContext_unref(presentation);
	}

	return ret;
}