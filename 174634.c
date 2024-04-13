static int ttusb_dec_init_dvb(struct ttusb_dec *dec)
{
	int result;

	dprintk("%s\n", __func__);

	if ((result = dvb_register_adapter(&dec->adapter,
					   dec->model_name, THIS_MODULE,
					   &dec->udev->dev,
					   adapter_nr)) < 0) {
		printk("%s: dvb_register_adapter failed: error %d\n",
		       __func__, result);

		return result;
	}

	dec->demux.dmx.capabilities = DMX_TS_FILTERING | DMX_SECTION_FILTERING;

	dec->demux.priv = (void *)dec;
	dec->demux.filternum = 31;
	dec->demux.feednum = 31;
	dec->demux.start_feed = ttusb_dec_start_feed;
	dec->demux.stop_feed = ttusb_dec_stop_feed;
	dec->demux.write_to_decoder = NULL;

	if ((result = dvb_dmx_init(&dec->demux)) < 0) {
		printk("%s: dvb_dmx_init failed: error %d\n", __func__,
		       result);

		dvb_unregister_adapter(&dec->adapter);

		return result;
	}

	dec->dmxdev.filternum = 32;
	dec->dmxdev.demux = &dec->demux.dmx;
	dec->dmxdev.capabilities = 0;

	if ((result = dvb_dmxdev_init(&dec->dmxdev, &dec->adapter)) < 0) {
		printk("%s: dvb_dmxdev_init failed: error %d\n",
		       __func__, result);

		dvb_dmx_release(&dec->demux);
		dvb_unregister_adapter(&dec->adapter);

		return result;
	}

	dec->frontend.source = DMX_FRONTEND_0;

	if ((result = dec->demux.dmx.add_frontend(&dec->demux.dmx,
						  &dec->frontend)) < 0) {
		printk("%s: dvb_dmx_init failed: error %d\n", __func__,
		       result);

		dvb_dmxdev_release(&dec->dmxdev);
		dvb_dmx_release(&dec->demux);
		dvb_unregister_adapter(&dec->adapter);

		return result;
	}

	if ((result = dec->demux.dmx.connect_frontend(&dec->demux.dmx,
						      &dec->frontend)) < 0) {
		printk("%s: dvb_dmx_init failed: error %d\n", __func__,
		       result);

		dec->demux.dmx.remove_frontend(&dec->demux.dmx, &dec->frontend);
		dvb_dmxdev_release(&dec->dmxdev);
		dvb_dmx_release(&dec->demux);
		dvb_unregister_adapter(&dec->adapter);

		return result;
	}

	dvb_net_init(&dec->adapter, &dec->dvb_net, &dec->demux.dmx);

	return 0;
}