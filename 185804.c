static void CALLBACK rfx_compose_message_tile_work_callback(PTP_CALLBACK_INSTANCE instance,
                                                            void* context, PTP_WORK work)
{
	RFX_TILE_COMPOSE_WORK_PARAM* param = (RFX_TILE_COMPOSE_WORK_PARAM*)context;
	rfx_encode_rgb(param->context, param->tile);
}