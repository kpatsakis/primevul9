static int tw5864_video_input_init(struct tw5864_input *input, int video_nr)
{
	struct tw5864_dev *dev = input->root;
	int ret;
	struct v4l2_ctrl_handler *hdl = &input->hdl;

	mutex_init(&input->lock);
	spin_lock_init(&input->slock);

	/* setup video buffers queue */
	INIT_LIST_HEAD(&input->active);
	input->vidq.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	input->vidq.timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
	input->vidq.io_modes = VB2_MMAP | VB2_READ | VB2_DMABUF;
	input->vidq.ops = &tw5864_video_qops;
	input->vidq.mem_ops = &vb2_dma_contig_memops;
	input->vidq.drv_priv = input;
	input->vidq.gfp_flags = 0;
	input->vidq.buf_struct_size = sizeof(struct tw5864_buf);
	input->vidq.lock = &input->lock;
	input->vidq.min_buffers_needed = 2;
	input->vidq.dev = &input->root->pci->dev;
	ret = vb2_queue_init(&input->vidq);
	if (ret)
		goto free_mutex;

	input->vdev = tw5864_video_template;
	input->vdev.v4l2_dev = &input->root->v4l2_dev;
	input->vdev.lock = &input->lock;
	input->vdev.queue = &input->vidq;
	video_set_drvdata(&input->vdev, input);

	/* Initialize the device control structures */
	v4l2_ctrl_handler_init(hdl, 6);
	v4l2_ctrl_new_std(hdl, &tw5864_ctrl_ops,
			  V4L2_CID_BRIGHTNESS, -128, 127, 1, 0);
	v4l2_ctrl_new_std(hdl, &tw5864_ctrl_ops,
			  V4L2_CID_CONTRAST, 0, 255, 1, 100);
	v4l2_ctrl_new_std(hdl, &tw5864_ctrl_ops,
			  V4L2_CID_SATURATION, 0, 255, 1, 128);
	v4l2_ctrl_new_std(hdl, &tw5864_ctrl_ops, V4L2_CID_HUE, -128, 127, 1, 0);
	v4l2_ctrl_new_std(hdl, &tw5864_ctrl_ops, V4L2_CID_MPEG_VIDEO_GOP_SIZE,
			  1, MAX_GOP_SIZE, 1, GOP_SIZE);
	v4l2_ctrl_new_std(hdl, &tw5864_ctrl_ops,
			  V4L2_CID_MPEG_VIDEO_H264_MIN_QP, 28, 51, 1, QP_VALUE);
	v4l2_ctrl_new_std_menu(hdl, &tw5864_ctrl_ops,
			       V4L2_CID_DETECT_MD_MODE,
			       V4L2_DETECT_MD_MODE_THRESHOLD_GRID, 0,
			       V4L2_DETECT_MD_MODE_DISABLED);
	v4l2_ctrl_new_std(hdl, &tw5864_ctrl_ops,
			  V4L2_CID_DETECT_MD_GLOBAL_THRESHOLD,
			  tw5864_md_thresholds.min, tw5864_md_thresholds.max,
			  tw5864_md_thresholds.step, tw5864_md_thresholds.def);
	input->md_threshold_grid_ctrl =
		v4l2_ctrl_new_custom(hdl, &tw5864_md_thresholds, NULL);
	if (hdl->error) {
		ret = hdl->error;
		goto free_v4l2_hdl;
	}
	input->vdev.ctrl_handler = hdl;
	v4l2_ctrl_handler_setup(hdl);

	input->qp = QP_VALUE;
	input->gop = GOP_SIZE;
	input->frame_interval = 1;

	ret = video_register_device(&input->vdev, VFL_TYPE_GRABBER, video_nr);
	if (ret)
		goto free_v4l2_hdl;

	dev_info(&input->root->pci->dev, "Registered video device %s\n",
		 video_device_node_name(&input->vdev));

	/*
	 * Set default video standard. Doesn't matter which, the detected value
	 * will be found out by VIDIOC_QUERYSTD handler.
	 */
	input->v4l2_std = V4L2_STD_NTSC_M;
	input->std = STD_NTSC;

	tw_indir_writeb(TW5864_INDIR_VIN_E(video_nr), 0x07);
	/* to initiate auto format recognition */
	tw_indir_writeb(TW5864_INDIR_VIN_F(video_nr), 0xff);

	return 0;

free_v4l2_hdl:
	v4l2_ctrl_handler_free(hdl);
	vb2_queue_release(&input->vidq);
free_mutex:
	mutex_destroy(&input->lock);

	return ret;
}