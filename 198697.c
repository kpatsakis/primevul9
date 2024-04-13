void tw5864_prepare_frame_headers(struct tw5864_input *input)
{
	struct tw5864_buf *vb = input->vb;
	u8 *dst;
	size_t dst_space;
	unsigned long flags;

	if (!vb) {
		spin_lock_irqsave(&input->slock, flags);
		if (list_empty(&input->active)) {
			spin_unlock_irqrestore(&input->slock, flags);
			input->vb = NULL;
			return;
		}
		vb = list_first_entry(&input->active, struct tw5864_buf, list);
		list_del(&vb->list);
		spin_unlock_irqrestore(&input->slock, flags);
	}

	dst = vb2_plane_vaddr(&vb->vb.vb2_buf, 0);
	dst_space = vb2_plane_size(&vb->vb.vb2_buf, 0);

	/*
	 * Low-level bitstream writing functions don't have a fine way to say
	 * correctly that supplied buffer is too small. So we just check there
	 * and warn, and don't care at lower level.
	 * Currently all headers take below 32 bytes.
	 * The buffer is supposed to have plenty of free space at this point,
	 * anyway.
	 */
	if (WARN_ON_ONCE(dst_space < 128))
		return;

	/*
	 * Generate H264 headers:
	 * If this is first frame, put SPS and PPS
	 */
	if (input->frame_gop_seqno == 0)
		tw5864_h264_put_stream_header(&dst, &dst_space, input->qp,
					      input->width, input->height);

	/* Put slice header */
	tw5864_h264_put_slice_header(&dst, &dst_space, input->h264_idr_pic_id,
				     input->frame_gop_seqno,
				     &input->tail_nb_bits, &input->tail);
	input->vb = vb;
	input->buf_cur_ptr = dst;
	input->buf_cur_space_left = dst_space;
}