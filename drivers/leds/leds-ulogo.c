/*
 * LEDs driver for ulogo
 *
 * Copyright (C) 2021 Ubiquiti inc.
  *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/leds.h>
#include <linux/timer.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/slab.h>

#define ULOGO_PATTERN_STR_MAX 256

int ledtrig_external(int index, int on);

struct ulogo_led_delay {
	unsigned int msec;
	uint32_t trg_mask;
};

struct ulogo_led_ctrl {
	struct led_classdev cdev;
	char pattern_str[ULOGO_PATTERN_STR_MAX];
	unsigned char delay_idx;
	unsigned char delay_total;
	struct ulogo_led_delay delay[CONFIG_LEDS_ULOGO_PATTERN_MAX];
	struct timer_list delay_timer;
};

static inline struct ulogo_led_ctrl *
cdev_to_ulogo_led_ctrl(struct led_classdev *led_cdev)
{
	return container_of(led_cdev, struct ulogo_led_ctrl, cdev);
}

static ssize_t ulogo_pattern_show(struct device *dev,
				  struct device_attribute *devattr, char *buf)
{
	struct led_classdev *cdev = dev_get_drvdata(dev);
	struct ulogo_led_ctrl *ctrl = cdev_to_ulogo_led_ctrl(cdev);

	return scnprintf(buf, PAGE_SIZE, "%s\n", ctrl->pattern_str);
}

static void strcpy_end_check(char *dst, const char *src, int copy_size)
{
	memcpy(dst, src, copy_size);
	if (dst[copy_size - 1] == '\n')
		dst[copy_size - 1] = '\0';
	else
		dst[copy_size] = '\0';
}

static void ulogo_pattern_set_timer(struct device *dev,
				    struct ulogo_led_ctrl *ctrl,
				    const char *buf)
{
	const char *cur = buf;
	struct ulogo_led_delay *delay;

	del_timer_sync(&ctrl->delay_timer);

	ctrl->delay_total = 0;
	while (*cur != '\0' &&
	       ctrl->delay_total < CONFIG_LEDS_ULOGO_PATTERN_MAX) {
		while (*cur == ' ')
			cur++;

		delay = &ctrl->delay[ctrl->delay_total];

		if (2 != sscanf(cur, "%u:%u", &delay->trg_mask, &delay->msec))
			break;

		dev_dbg(dev, "%u - %u\n", delay->trg_mask, delay->msec);

		ctrl->delay_total += 1;

		while (*cur != ' ' && *cur != '\0')
			cur++;
	}

	if (ctrl->delay_total == 0)
		return;

	ctrl->delay_idx = 0;

	mod_timer(&ctrl->delay_timer, jiffies + 1);
}

static ssize_t ulogo_pattern_store(struct device *dev,
				   struct device_attribute *attr,
				   const char *buf, size_t size)
{
	struct led_classdev *cdev = dev_get_drvdata(dev);
	struct ulogo_led_ctrl *ctrl = cdev_to_ulogo_led_ctrl(cdev);
	int copy_size = min(size, sizeof(ctrl->pattern_str) - 1);

	strcpy_end_check(ctrl->pattern_str, buf, copy_size);
	ulogo_pattern_set_timer(dev, ctrl, buf);

	return size;
}

static void ulogo_constant_set(struct device *dev, struct ulogo_led_ctrl *ctrl,
			       uint32_t trg_mask)
{
	int i;

	del_timer_sync(&ctrl->delay_timer);

	for (i = 0; i < CONFIG_LEDS_TRIGGER_EXTERNAL_MAX; i++)
		ledtrig_external(i, (trg_mask & (1 << i)) ? LED_ON : LED_OFF);
}

static ssize_t ulogo_constant_store(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t size)
{
	struct led_classdev *cdev = dev_get_drvdata(dev);
	struct ulogo_led_ctrl *ctrl = cdev_to_ulogo_led_ctrl(cdev);
	uint32_t trg_mask;
	int ret;
	int copy_size = min(size, sizeof(ctrl->pattern_str));

	ret = sscanf(buf, "%u", &trg_mask);
	if (ret != 1) {
		dev_err(dev, "fail to get led trg_mask\n");
		return -EINVAL;
	}

	strcpy_end_check(ctrl->pattern_str, buf, copy_size);
	ulogo_constant_set(dev, ctrl, trg_mask);

	return size;
}

static void ulogo_binary_to_pattern(struct device *dev,
				    struct ulogo_led_ctrl *ctrl,
				    unsigned int max_bits, unsigned int number)
{
	struct ulogo_led_delay *delay;

	del_timer_sync(&ctrl->delay_timer);

	// Convert number to binary number and turn on correspond led by each bit
	// in reverse order
	// e.g. 6 --> 110 --> 1:150 0:150 1:150 0:150 2:150 0:850
	ctrl->delay_total = 0;
	while (ctrl->delay_total < max_bits) {
		delay = &ctrl->delay[max_bits - (ctrl->delay_total++) - 1];
		delay->trg_mask = 0;
		delay->msec = (ctrl->delay_total == 1) ? 850 : 150;

		delay = &ctrl->delay[max_bits - (ctrl->delay_total++) - 1];
		delay->trg_mask = (number & 1) ? 0x1 : 0x2;
		delay->msec = 150;

		number >>= 1;
	}
	ctrl->delay_idx = 0;

	if (ctrl->delay_total == 0)
		return;

	mod_timer(&ctrl->delay_timer, jiffies + 1);
}

static ssize_t ulogo_binary_store(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf, size_t size)
{
	struct led_classdev *cdev = dev_get_drvdata(dev);
	struct ulogo_led_ctrl *ctrl = cdev_to_ulogo_led_ctrl(cdev);
	unsigned int max_bits, number;
	int ret;
	int copy_size = min(size, sizeof(ctrl->pattern_str));

	ret = sscanf(buf, "%u %u", &max_bits, &number);
	if (ret != 2) {
		dev_err(dev, "fail to get number\n");
		return -EINVAL;
	}

	// one for on, and one for off
	max_bits *= 2;

	if (max_bits > CONFIG_LEDS_ULOGO_PATTERN_MAX)
		return -E2BIG;

	strcpy_end_check(ctrl->pattern_str, buf, copy_size);
	ulogo_binary_to_pattern(dev, ctrl, max_bits, number);

	return size;
}

static DEVICE_ATTR(pattern, 0644, ulogo_pattern_show, ulogo_pattern_store);
static DEVICE_ATTR(constant, 0644, ulogo_pattern_show, ulogo_constant_store);
static DEVICE_ATTR(binary, 0644, ulogo_pattern_show, ulogo_binary_store);

static struct attribute *ulogo_attrs[] = {
	&dev_attr_pattern.attr,
	&dev_attr_constant.attr,
	&dev_attr_binary.attr,
	NULL,
};

ATTRIBUTE_GROUPS(ulogo);

static void led_pattern_timer_function(struct timer_list *t)
{
	struct ulogo_led_ctrl *ctrl = from_timer(ctrl, t, delay_timer);
	struct ulogo_led_delay *cur_led = &ctrl->delay[ctrl->delay_idx];
	int i;

	for (i = 0; i < CONFIG_LEDS_TRIGGER_EXTERNAL_MAX; i++)
		ledtrig_external(i, (cur_led->trg_mask & (1 << i)) ? LED_ON :
								     LED_OFF);

	mod_timer(&ctrl->delay_timer,
		  jiffies + msecs_to_jiffies(cur_led->msec));

	ctrl->delay_idx += 1;
	if (ctrl->delay_idx >= ctrl->delay_total)
		ctrl->delay_idx = 0;
}

static void ctrl_led_set_brightness(struct led_classdev *led_cdev,
				    enum led_brightness value)
{
	struct ulogo_led_ctrl *ctrl = cdev_to_ulogo_led_ctrl(led_cdev);
	int i;

	del_timer_sync(&ctrl->delay_timer);

	for (i = 0; i < CONFIG_LEDS_TRIGGER_EXTERNAL_MAX; i++)
		ledtrig_external(i, value);
}

static struct ulogo_led_ctrl *leds_ulogo_create(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct ulogo_led_ctrl *ctrl;
	int ret;
	const char *default_pattern = NULL;

	ctrl = devm_kzalloc(dev, sizeof(*ctrl), GFP_KERNEL);
	if (!ctrl)
		return ERR_PTR(-ENOMEM);

	ret = device_property_read_string(dev, "label", &ctrl->cdev.name);
	if (ret && IS_ENABLED(CONFIG_OF) && dev->of_node)
		ctrl->cdev.name = dev->of_node->name;
	if (!ctrl->cdev.name)
		return ERR_PTR(-EINVAL);

	ctrl->cdev.groups = ulogo_groups;
	ctrl->cdev.brightness_set = ctrl_led_set_brightness;

	timer_setup(&ctrl->delay_timer, led_pattern_timer_function, 0);

	ret = devm_of_led_classdev_register(dev, dev->of_node, &ctrl->cdev);
	if (ret < 0)
		return ERR_PTR(ret);

	if (!device_property_read_string(dev, "default_pattern",
					 &default_pattern))
		ulogo_pattern_set_timer(dev, ctrl, default_pattern);

	return ctrl;
}

static const struct of_device_id of_leds_ulogo_match[] = {
	{ .compatible = "leds-ulogo" },
	{},
};

MODULE_DEVICE_TABLE(of, of_leds_ulogo_match);

static int leds_ulogo_probe(struct platform_device *pdev)
{
	struct ulogo_led_ctrl *ctrl;

	ctrl = leds_ulogo_create(pdev);
	if (IS_ERR(ctrl))
		return PTR_ERR(ctrl);

	platform_set_drvdata(pdev, ctrl);

	return 0;
}

static int leds_ulogo_remove(struct platform_device *pdev)
{
	struct ulogo_led_ctrl *ctrl = platform_get_drvdata(pdev);

	del_timer_sync(&ctrl->delay_timer);
	return 0;
}

static struct platform_driver gpio_led_driver = {
	.probe		= leds_ulogo_probe,
	.remove		= leds_ulogo_remove,
	.driver		= {
		.name	= "leds-ulogo",
		.of_match_table = of_leds_ulogo_match,
	},
};

module_platform_driver(gpio_led_driver);

MODULE_AUTHOR("Ken Lu <ken.lu@ui.com>");
MODULE_DESCRIPTION("GPIO ULOGO LED driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:leds-ulogo");
