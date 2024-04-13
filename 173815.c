static void hidpp10_extra_mouse_buttons_populate_input(
			struct hidpp_device *hidpp, struct input_dev *input_dev)
{
	/* BTN_MOUSE - BTN_MOUSE+7 are set already by the descriptor */
	__set_bit(BTN_0, input_dev->keybit);
	__set_bit(BTN_1, input_dev->keybit);
	__set_bit(BTN_2, input_dev->keybit);
	__set_bit(BTN_3, input_dev->keybit);
	__set_bit(BTN_4, input_dev->keybit);
	__set_bit(BTN_5, input_dev->keybit);
	__set_bit(BTN_6, input_dev->keybit);
	__set_bit(BTN_7, input_dev->keybit);
}