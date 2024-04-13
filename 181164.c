static int ad5755_set_channel_pwr_down(struct iio_dev *indio_dev,
	unsigned int channel, bool pwr_down)
{
	struct ad5755_state *st = iio_priv(indio_dev);
	unsigned int mask = BIT(channel);

	mutex_lock(&indio_dev->mlock);

	if ((bool)(st->pwr_down & mask) == pwr_down)
		goto out_unlock;

	if (!pwr_down) {
		st->pwr_down &= ~mask;
		ad5755_update_dac_ctrl(indio_dev, channel,
			AD5755_DAC_INT_EN | AD5755_DAC_DC_DC_EN, 0);
		udelay(200);
		ad5755_update_dac_ctrl(indio_dev, channel,
			AD5755_DAC_OUT_EN, 0);
	} else {
		st->pwr_down |= mask;
		ad5755_update_dac_ctrl(indio_dev, channel,
			0, AD5755_DAC_INT_EN | AD5755_DAC_OUT_EN |
				AD5755_DAC_DC_DC_EN);
	}

out_unlock:
	mutex_unlock(&indio_dev->mlock);

	return 0;
}