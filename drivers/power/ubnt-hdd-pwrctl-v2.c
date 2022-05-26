/*
 *  Copyright (C) 2020, Matt Hsu <matt.hsu@ui.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under  the terms of the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/workqueue.h>
#include <linux/sysfs.h>
#include <linux/delay.h>

#include <linux/power/ubnt-hdd-pwrctl.h>

#define UBNT_HDD_PWRCTL_DELAY (HZ * 2)

struct ubnt_hdd_pwrctl_v2 {
	struct device *dev;

	struct delayed_work timeout;
	struct list_head pdata_list;
	struct ubnt_hdd_pwrctl_platform_data *pdata;
};

static void
ubnt_hdd_pwrctl_v2_work(struct work_struct *work)
{
	struct ubnt_hdd_pwrctl_v2 *pwrctl = container_of(work,
							struct ubnt_hdd_pwrctl_v2, timeout.work);
	struct ubnt_hdd_pwrctl_platform_data *pdata;
	int ret;

	pdata = pwrctl->pdata;

	list_for_each_entry(pdata, &pwrctl->pdata_list, list) {
		/* present_gpio is LOW active, present_state */
		dev_dbg(pwrctl->dev, "present_gpio# %d, present_gpio %d, present_state %d\n",
				pdata->present_gpio,
				gpio_get_value_cansleep(pdata->present_gpio),
				pdata->present_state);

		if ((!gpio_get_value_cansleep(pdata->present_gpio)) != pdata->present_state) {
			msleep(pdata->pwren_delay);

			ret = gpio_direction_output(pdata->pwren_gpio, !pdata->present_state);
			if (ret) {
				dev_err(pwrctl->dev, "Failed to set gpio# %d to output: %d\n",
								pdata->pwren_gpio, ret);
			}
			pdata->present_state = !pdata->present_state;
			dev_dbg(pwrctl->dev, "Set pwren_gpio# %d as %d, update state %d\n",
						pdata->pwren_gpio,
						!pdata->present_state,
						pdata->present_state);
		}
	}

	schedule_delayed_work(&pwrctl->timeout, UBNT_HDD_PWRCTL_DELAY);
}

static struct ubnt_hdd_pwrctl_platform_data *
ubnt_hdd_pwrctl_parse_dt(struct device *dev, struct ubnt_hdd_pwrctl_v2 *pwrctl)
{
	struct device_node *np = dev->of_node;
	struct ubnt_hdd_pwrctl_platform_data *pdata = NULL;
	int ret;

	if (!np)
		return ERR_PTR(-ENOENT);

	np = of_find_compatible_node(NULL, NULL, "ubnt, hdd-pwrctl-v2");
	if (np) {
		struct device_node *child;

		INIT_LIST_HEAD(&pwrctl->pdata_list);
		for_each_child_of_node(np, child) {
			pdata = devm_kzalloc(dev, sizeof(*pdata), GFP_KERNEL);
			if (!pdata) {
				of_node_put(child);
				of_node_put(np);
				return ERR_PTR(-ENOMEM);
			}

			strncpy(pdata->name, of_node_full_name(child), 32);
			ret = of_property_read_u32(child, "hdds-pwren", &pdata->pwren_gpio);
			if (ret) {
				dev_err(dev, "failed to get hdds-pwren %d\n", ret);
			}

			ret = of_property_read_u32(child, "hdds-present", &pdata->present_gpio);
			if (ret) {
				dev_err(dev, "failed to get hdds-present %d\n", ret);
			}

			ret = of_property_read_u32(child, "hdds-fault-led", &pdata->fault_led_gpio);
			if (ret) {
				dev_err(dev, "failed to get hdds-fault-led %d\n", ret);
			}

			ret = of_property_read_u32(child, "pwren-delay", &pdata->pwren_delay);
			if (ret) {
				dev_err(dev, "failed to get power enable delay %d, use default\n", ret);
				pdata->pwren_delay = 500;
			}

			pdata->fault_state = false;
			pdata->present_state = false;
			dev_info(dev, "name %s, pwren %u, present %u, fault-led %u, pwren delay %u\n",
					pdata->name,
					pdata->pwren_gpio,
					pdata->present_gpio,
					pdata->fault_led_gpio,
					pdata->pwren_delay);
			list_add_tail(&pdata->list, &pwrctl->pdata_list);
		}
		of_node_put(np);
	}

	return pdata;
}

static ssize_t show_hdd_present(struct device *dev,
                struct device_attribute *attr, char *buf)
{
	struct ubnt_hdd_pwrctl_v2 *pwrctl = dev_get_drvdata(dev);
	struct ubnt_hdd_pwrctl_platform_data *pdata = pwrctl->pdata;
	char present_state[32];
	char *p = present_state;

	list_for_each_entry(pdata, &pwrctl->pdata_list, list) {
		if (pdata->present_state)
			p += sprintf(p, "%s ", pdata->name + (strlen(pdata->name) - 1));
	}
	p += sprintf(p, "\n");

	return snprintf(buf, PAGE_SIZE, "%s", present_state);
}

static ssize_t show_hdd_fault_led(struct device *dev,
                struct device_attribute *attr, char *buf)
{
	struct ubnt_hdd_pwrctl_v2 *pwrctl = dev_get_drvdata(dev);
	struct ubnt_hdd_pwrctl_platform_data *pdata = pwrctl->pdata;
	char fault_state[32];
	char *p = fault_state;

	list_for_each_entry(pdata, &pwrctl->pdata_list, list) {
		if (pdata->fault_state)
			p += sprintf(p, "%s ", pdata->name + (strlen(pdata->name) - 1));
	}
	p += sprintf(p, "\n");

	return snprintf(buf, PAGE_SIZE, "%s", fault_state);
}

static ssize_t store_hdd_fault_led(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	struct ubnt_hdd_pwrctl_v2 *pwrctl = dev_get_drvdata(dev);
	struct ubnt_hdd_pwrctl_platform_data *pdata = pwrctl->pdata;

	list_for_each_entry(pdata, &pwrctl->pdata_list, list) {
		if (!strncmp(pdata->name + (strlen(pdata->name) - 1), buf, 1)) {
			pdata->fault_state = true;
			ret = gpio_direction_output(pdata->fault_led_gpio, 1);
			if (ret) {
				dev_err(pwrctl->dev, "Failed to set gpio# %d to output: %d\n",
						pdata->fault_led_gpio, ret);
			}
		}
	}

	return count;
}

static ssize_t store_hdd_fault_ledoff(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	struct ubnt_hdd_pwrctl_v2 *pwrctl = dev_get_drvdata(dev);
	struct ubnt_hdd_pwrctl_platform_data *pdata = pwrctl->pdata;

	list_for_each_entry(pdata, &pwrctl->pdata_list, list) {
		if (!strncmp(pdata->name + (strlen(pdata->name) - 1), buf, 1)) {
			pdata->fault_state = false;
			ret = gpio_direction_output(pdata->fault_led_gpio, 0);
			if (ret) {
				dev_err(pwrctl->dev, "Failed to set gpio# %d to output: %d\n",
						pdata->fault_led_gpio, ret);
			}
		}
	}

	return count;
}

static ssize_t show_hdd_power(struct device *dev,
                struct device_attribute *attr, char *buf)
{
	struct ubnt_hdd_pwrctl_v2 *pwrctl = dev_get_drvdata(dev);
	struct ubnt_hdd_pwrctl_platform_data *pdata = pwrctl->pdata;
	char power_state[32];
	char *p = power_state;

	list_for_each_entry(pdata, &pwrctl->pdata_list, list) {
		if (gpio_get_value_cansleep(pdata->pwren_gpio))
			p += sprintf(p, "%s ", pdata->name + (strlen(pdata->name) - 1));
	}

	p += sprintf(p, "\n");
	return snprintf(buf, PAGE_SIZE, "%s", power_state);
}

static ssize_t force_poweron(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	struct ubnt_hdd_pwrctl_v2 *pwrctl = dev_get_drvdata(dev);
	struct ubnt_hdd_pwrctl_platform_data *pdata = pwrctl->pdata;

	list_for_each_entry(pdata, &pwrctl->pdata_list, list) {
		if (!strncmp(pdata->name + (strlen(pdata->name) - 1), buf, 1)) {
			ret = gpio_direction_output(pdata->pwren_gpio, 1);
			if (ret) {
				dev_err(pwrctl->dev, "Failed to set gpio# %d to output: %d\n",
						pdata->pwren_gpio, ret);
			}
		}
	}

	return count;
}

static ssize_t force_poweroff(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	struct ubnt_hdd_pwrctl_v2 *pwrctl = dev_get_drvdata(dev);
	const struct ubnt_hdd_pwrctl_platform_data *pdata = pwrctl->pdata;

	list_for_each_entry(pdata, &pwrctl->pdata_list, list) {
		if (!strncmp(pdata->name + (strlen(pdata->name) - 1), buf, 1)) {
			ret = gpio_direction_output(pdata->pwren_gpio, 0);
			if (ret) {
				dev_err(pwrctl->dev, "Failed to set gpio# %d to output: %d\n",
						pdata->pwren_gpio, ret);
			}
		}
	}

	return count;
}

static DEVICE_ATTR(hdd_present, S_IRUSR, show_hdd_present, NULL);
static DEVICE_ATTR(hdd_fault_led, S_IRUGO | S_IWUSR, show_hdd_fault_led, store_hdd_fault_led);
static DEVICE_ATTR(hdd_fault_ledoff, S_IWUSR, NULL, store_hdd_fault_ledoff);
static DEVICE_ATTR(hdd_power, S_IRUSR, show_hdd_power, NULL);
static DEVICE_ATTR(hdd_force_poweron, S_IWUSR, NULL, force_poweron);
static DEVICE_ATTR(hdd_force_poweroff, S_IWUSR, NULL, force_poweroff);

static struct attribute *ubnt_hdd_pwrctl_attr[] = {
    &dev_attr_hdd_present.attr,
    &dev_attr_hdd_fault_led.attr,
    &dev_attr_hdd_fault_ledoff.attr,
    &dev_attr_hdd_power.attr,
    &dev_attr_hdd_force_poweron.attr,
    &dev_attr_hdd_force_poweroff.attr,
    NULL,
};

static const struct attribute_group ubnt_hdd_pwrctl_attr_group = {
    .attrs = ubnt_hdd_pwrctl_attr,
};

struct ubnt_hdd_pwrctl_v2 *g_pwrctl = NULL;

int ubnt_hdd_fault_led_enabled(int fault_led_gpio)
{
	const struct ubnt_hdd_pwrctl_platform_data *pdata;

	if (!g_pwrctl)
		return 0;

	pdata = g_pwrctl->pdata;

	list_for_each_entry(pdata, &g_pwrctl->pdata_list, list) {
		if (pdata->fault_led_gpio == fault_led_gpio)
			return pdata->fault_state;
	}

	return 0;
}
EXPORT_SYMBOL(ubnt_hdd_fault_led_enabled);

static int ubnt_hdd_pwrctl_v2_probe(struct platform_device *pdev)
{
	int ret;
	struct ubnt_hdd_pwrctl_platform_data *pdata = pdev->dev.platform_data;
	struct ubnt_hdd_pwrctl_v2 *pwrctl;

	pwrctl = devm_kzalloc(&pdev->dev, sizeof(*pwrctl), GFP_KERNEL);
	if (!pwrctl) {
		dev_err(&pdev->dev, "Failed to alloc driver structure\n");
		return -ENOMEM;
	}

	if (!pdata) {
		pdata = ubnt_hdd_pwrctl_parse_dt(&pdev->dev, pwrctl);
		if (IS_ERR(pdata)) {
			ret = PTR_ERR(pdata);
			if (ret != -EPROBE_DEFER)
				dev_err(&pdev->dev, "No platform data\n");
			return ret;
		}
	}

	INIT_DELAYED_WORK(&pwrctl->timeout, ubnt_hdd_pwrctl_v2_work);

	list_for_each_entry(pdata, &pwrctl->pdata_list, list) {
		ret = gpio_request(pdata->present_gpio, pdata->name);
		if (ret) {
			dev_err(&pdev->dev, "Failed to request gpio %u pin: %d\n", pdata->present_gpio, ret);
			goto err_free;
		}

		ret = gpio_direction_input(pdata->present_gpio);
		if (ret) {
			dev_err(&pdev->dev, "Failed to set gpio to input: %d\n", ret);
			goto err_gpio_present_free;
		}
		ret = gpio_request(pdata->pwren_gpio, pdata->name);
		if (ret) {
			dev_err(&pdev->dev, "Failed to request gpio %u pin: %d\n", pdata->pwren_gpio, ret);
			goto err_gpio_present_free;
		}

		ret = gpio_request(pdata->fault_led_gpio, pdata->name);
		if (ret) {
			dev_err(&pdev->dev, "Failed to request gpio %u pin: %d\n", pdata->fault_led_gpio, ret);
			goto err_gpio_pwren_free;
		}
	}

	ret = sysfs_create_group(&pdev->dev.kobj, &ubnt_hdd_pwrctl_attr_group);
	if (ret) {
		dev_err(&pdev->dev, "Failed to create sysfs group\n");
		goto err_gpio_fault_led_free;
	}

	pwrctl->dev = &pdev->dev;
	pwrctl->pdata = pdata;
	platform_set_drvdata(pdev, pwrctl);
	g_pwrctl = pwrctl;

	schedule_delayed_work(&pwrctl->timeout, UBNT_HDD_PWRCTL_DELAY);

	return 0;

err_gpio_fault_led_free:
	list_for_each_entry(pdata, &pwrctl->pdata_list, list){
		gpio_free(pdata->fault_led_gpio);
	}
err_gpio_pwren_free:
	list_for_each_entry(pdata, &pwrctl->pdata_list, list){
		gpio_free(pdata->pwren_gpio);
	}
err_gpio_present_free:
	list_for_each_entry(pdata, &pwrctl->pdata_list, list){
		gpio_free(pdata->present_gpio);
	}
err_free:
	return ret;
}

static int ubnt_hdd_pwrctl_v2_remove(struct platform_device *pdev)
{
	struct ubnt_hdd_pwrctl_platform_data *pdata = pdev->dev.platform_data;
	struct ubnt_hdd_pwrctl_v2 *pwrctl = platform_get_drvdata(pdev);

	list_for_each_entry(pdata, &pwrctl->pdata_list, list){
		gpio_free(pdata->fault_led_gpio);
		gpio_free(pdata->pwren_gpio);
		gpio_free(pdata->present_gpio);
	}

	flush_delayed_work(&pwrctl->timeout);
	return 0;
}

static const struct of_device_id ubnt_hdd_pwrctl_v2_match[] = {
	{ .compatible = "ubnt, hdd-pwrctl-v2" },
	{ }
};
MODULE_DEVICE_TABLE(of, ubnt_hdd_pwrctl_v2_match);

static struct platform_driver ubnt_hdd_pwrctl_v2_driver = {
	.probe = ubnt_hdd_pwrctl_v2_probe,
	.remove = ubnt_hdd_pwrctl_v2_remove,
	.driver = {
		.name = "ubnt-hdd-pwrctl-v2",
		.of_match_table = ubnt_hdd_pwrctl_v2_match,
	},
};

module_platform_driver(ubnt_hdd_pwrctl_v2_driver);
