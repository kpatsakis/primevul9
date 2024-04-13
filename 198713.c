static void tw5864_handle_frame_task(unsigned long data)
{
	struct tw5864_dev *dev = (struct tw5864_dev *)data;
	unsigned long flags;
	int batch_size = H264_BUF_CNT;

	spin_lock_irqsave(&dev->slock, flags);
	while (dev->h264_buf_r_index != dev->h264_buf_w_index && batch_size--) {
		struct tw5864_h264_frame *frame =
			&dev->h264_buf[dev->h264_buf_r_index];

		spin_unlock_irqrestore(&dev->slock, flags);
		dma_sync_single_for_cpu(&dev->pci->dev, frame->vlc.dma_addr,
					H264_VLC_BUF_SIZE, DMA_FROM_DEVICE);
		dma_sync_single_for_cpu(&dev->pci->dev, frame->mv.dma_addr,
					H264_MV_BUF_SIZE, DMA_FROM_DEVICE);
		tw5864_handle_frame(frame);
		dma_sync_single_for_device(&dev->pci->dev, frame->vlc.dma_addr,
					   H264_VLC_BUF_SIZE, DMA_FROM_DEVICE);
		dma_sync_single_for_device(&dev->pci->dev, frame->mv.dma_addr,
					   H264_MV_BUF_SIZE, DMA_FROM_DEVICE);
		spin_lock_irqsave(&dev->slock, flags);

		dev->h264_buf_r_index++;
		dev->h264_buf_r_index %= H264_BUF_CNT;
	}
	spin_unlock_irqrestore(&dev->slock, flags);
}