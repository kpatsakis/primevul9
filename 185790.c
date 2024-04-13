static void CALLBACK rfx_process_message_tile_work_callback(PTP_CALLBACK_INSTANCE instance,
                                                            void* context, PTP_WORK work)
{
	RFX_TILE_PROCESS_WORK_PARAM* param = (RFX_TILE_PROCESS_WORK_PARAM*)context;
	rfx_decode_rgb(param->context, param->tile, param->tile->data, 64 * 4);
}