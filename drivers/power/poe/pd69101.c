/* Copyright (c) 2017, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

#include <linux/err.h>
#include <linux/errno.h>
#include <linux/of_platform.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/hrtimer.h>

static const char *const ff_clk_name = "ff-clk-gpio";
static const char *const ff_d_name = "ff-d-gpio";

#define LED1_GPIO   ((pd69101)->gpio1)
#define LED0_GPIO   ((pd69101)->gpio0)
#define LED1_IRQ   ((pd69101)->irq1)
#define LED0_IRQ   ((pd69101)->irq0)


struct pd69101_data {
	struct platform_device          *pdev;
	int 	ff_clk_gpio;
	int 	ff_d_gpio;
	u32	onoff;
	unsigned long	time_stamp;
	int	irq0;
	int 	gpio1;
	int 	irq1;
	int	gpio0;
	int	state;
	int	blinking;
};


enum states
{
    ST_ANY = -1,
    ST_OFF,
    ST_AF_ON,
    ST_AT_ON,
    ST_FAILURE,
    ST_MAX_STATES
};

enum evs
{
    EV_ANY = -1,
    EV_INIT = 1000,
    EV_STATIC_UPDATE,
    EV_LED0BLINK,
    EV_MAX
};

typedef struct {
    int st;
    int ev;
    int (*fn)(struct pd69101_data *pd69101);
} tTransition;


static int fsm_static_update (struct pd69101_data *pd69101);
static int fsm_blink_update (struct pd69101_data *pd69101); 
static int fsm_error (struct pd69101_data *pd69101) {
	return ST_MAX_STATES;
};

tTransition trans[] = {
    { ST_OFF, EV_INIT, &fsm_static_update},
    { ST_FAILURE, EV_INIT, &fsm_static_update}, //put this before any state, only init could reset the state
    { ST_FAILURE, EV_ANY, &fsm_blink_update},
    { ST_ANY, EV_STATIC_UPDATE, &fsm_static_update},
    { ST_ANY, EV_LED0BLINK, &fsm_blink_update},
    { ST_ANY, EV_ANY, &fsm_error}
};
#define TRANS_COUNT (sizeof(trans)/sizeof(*trans))


static inline int is_led_on(int led)
{
        return !gpio_get_value_cansleep(led);
}


static int fsm_static_update (struct pd69101_data *pd69101) 
{
	if(is_led_on(LED0_GPIO) && is_led_on(LED1_GPIO)) {
		return ST_AT_ON;
	} else if(is_led_on(LED0_GPIO)) {
		return ST_AF_ON;
	}
	return ST_OFF;
};

static int fsm_blink_update (struct pd69101_data *pd69101) 
{
	return ST_FAILURE;
};

static int fsm_event_update(struct pd69101_data *pd69101, int event)
{
    int i;
    int state = pd69101->state;
    for (i = 0; i < TRANS_COUNT; i++) {
        if ((state == trans[i].st) || (ST_ANY == trans[i].st)) {
            if ((event == trans[i].ev) || (EV_ANY == trans[i].ev)) {
                state = (trans[i].fn)(pd69101);
                break;
            }
        }
    }
    pr_debug("pd69101 state %d -> %d\n", pd69101->state, state);
    pd69101->state = state;
    return state;
}


static int pd69101_reset_configure(struct pd69101_data *data, bool on)
{
        int ret = 0;
        int val1 = on ? 1:0;
        int val2 = on ? 0:1;

        ret = gpio_direction_output(data->ff_d_gpio, val1);
        if (ret) {
                pr_err("flip-flop data gpio out failed\n");
                return -EINVAL;
        }

        ret = gpio_direction_output(data->ff_clk_gpio, 0);
        if (ret) {
                pr_err("edge gpio out failed\n");
                return -EINVAL;
        }

        ret = gpio_direction_output(data->ff_clk_gpio, 1);
        if (ret) {
                pr_err("edge gpio out failed\n");
                return -EINVAL;
        }

        mdelay(10);
        ret = gpio_direction_output(data->ff_d_gpio, val2);
        if (ret) {
                pr_err("flip-flop data gpio out failed\n");
                return -EINVAL;
        }

        ret = gpio_direction_output(data->ff_clk_gpio, 0);
        if (ret) {
                pr_err("edge gpio out failed\n");
                return -EINVAL;
        }

        ret = gpio_direction_output(data->ff_clk_gpio, 1);
        if (ret) {
                pr_err("edge gpio out failed\n");
                return -EINVAL;
        }
	return ret;
}

static ssize_t reset_store(struct device *dev,
                struct device_attribute *attr, const char *buf, size_t count)
{
        struct pd69101_data *data = dev_get_drvdata(dev);
        int ret, mode;

        ret = kstrtoint(buf, 10, &mode);
        if (ret != 0) {
                pr_err("%s: ret = %d\n", __func__, ret);
                return -EINVAL;
        }

        if(mode == 1) {
                pd69101_reset_configure(data, true);
        } else if(mode == 0) {
                pd69101_reset_configure(data, false);
        }
        return count;
}

static DEVICE_ATTR_WO(reset);

static ssize_t state_store(struct device *dev,
                struct device_attribute *attr, const char *buf, size_t count)
{
	struct pd69101_data *pd69101 = dev_get_drvdata(dev);
	int ret, mode;

	ret = kstrtoint(buf, 10, &mode);
	if (ret != 0) {
		pr_err("%s: ret = %d\n", __func__, ret);
		return -EINVAL;
	}

	if(mode == 0) {
		//reset to init
		fsm_event_update(pd69101, EV_INIT);
	}
	return count;
}

static ssize_t state_show(struct device *dev,
                          struct device_attribute *attr,
                          char *buf)
{
	struct pd69101_data *data = dev_get_drvdata(dev);
	ssize_t len = 0;

	sprintf(buf, "%d\n", data->state);
	len = strlen(buf);
	return len;
}
static DEVICE_ATTR_RW(state);

#define BLINK_DURATION_MIN      500
#define BLINK_DURATION_MAX      525
#define BLINK_TO_FAILURE	4

static irqreturn_t pd69101_irq_t(int irq, void *pdata)
{
	unsigned long now = jiffies;
	struct pd69101_data *pd69101 = (struct pd69101_data *)pdata;

	pr_debug("irq %s msec %d \r\n", irq == LED0_IRQ ? "led0_irq":"led1_irq", jiffies_to_msecs(pd69101->time_stamp - now));
	//check possible blink on led0
	if(irq == LED0_IRQ) {
		if( now > (pd69101->time_stamp + msecs_to_jiffies(BLINK_DURATION_MIN)) &&
			now < (pd69101->time_stamp + msecs_to_jiffies(BLINK_DURATION_MAX))) {
				pd69101->blinking++;
				pr_debug("pd69101 blinking %d", pd69101->blinking);
				if(pd69101->blinking >= BLINK_TO_FAILURE)
					fsm_event_update(pd69101, EV_LED0BLINK);
		} else {
			if(pd69101->blinking >=BLINK_TO_FAILURE) {
				//assume pse state changed
				fsm_event_update(pd69101, EV_INIT);
			}
			pd69101->blinking = 0;
		}
		pd69101->time_stamp = now;

	}

	fsm_event_update(pd69101, EV_STATIC_UPDATE);

	return IRQ_HANDLED;

}

static int pd69101_gpio_alloc(unsigned pin, const char *label, bool is_in)
{
	int value;

	value = gpio_request(pin, label);
	if (value == 0) {
		if (is_in)
			value = gpio_direction_input(pin);
		else
			value = gpio_direction_output(pin, 0);
        }
	return value;
}

static int pd69101_led_gpio_config(struct pd69101_data *pd69101)
{
	int rc;
	struct platform_device *pdev = pd69101->pdev;
	struct device_node *of_node = pdev->dev.of_node;

	rc = of_get_named_gpio(of_node, "gpio-led1", 0);
	if (rc < 0) {
		dev_err(&pdev->dev, "gpio-led1 property not found\n");
	}
	LED1_GPIO = rc ? rc : 0;

	rc = of_get_named_gpio(of_node, "gpio-led0", 0);
	if (rc < 0) {
		dev_err(&pdev->dev, "gpio-led0 property not found\n");
	}
	LED0_GPIO = rc ? rc : 0;

	if(LED0_GPIO < 0 || LED1_GPIO <= 0) {
		return 0;
	}

	pd69101_gpio_alloc(LED1_GPIO, "led1_gpio", true);
	pd69101_gpio_alloc(LED0_GPIO, "led0_gpio", true);

	if (gpio_is_valid(LED1_GPIO)) {
		LED1_IRQ = gpio_to_irq(LED1_GPIO);
	}

	if (gpio_is_valid(LED0_GPIO)) {
		LED0_IRQ = gpio_to_irq(LED0_GPIO);
	}

	rc = request_threaded_irq(LED1_IRQ, NULL, pd69101_irq_t,
			IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_SHARED | IRQF_ONESHOT,
			"pd69101-irq", pd69101);
	if (rc) {
		pr_err("failed to request IRQ #%d\n", LED1_IRQ);
		return rc;
	}

	rc = request_threaded_irq(LED0_IRQ, NULL, pd69101_irq_t,
			IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING | IRQF_SHARED | IRQF_ONESHOT,
			"pd69101-irq", pd69101);
	if (rc) {
		pr_err("failed to request IRQ #%d\n", LED1_IRQ);
		return rc;
	}
	fsm_event_update(pd69101, EV_INIT);

	rc = sysfs_create_file(&pdev->dev.kobj,
				&dev_attr_state.attr);

	if (rc < 0) {
		dev_err(&pdev->dev, "sysfs creation failed\n");
	}

	return 0;
}

static int pd69101_ff_gpio_config(struct pd69101_data *pd69101)
{
	int rc;
	struct platform_device *pdev = pd69101->pdev;
	struct device_node *of_node = pdev->dev.of_node;

	pd69101->ff_clk_gpio = of_get_named_gpio(of_node, ff_clk_name, 0);
	if (pd69101->ff_clk_gpio < 0) {
		dev_err(&pdev->dev, "GPIO <%s> not found, ff-clk pin not used\n",
			ff_clk_name);
		rc = -ENODEV;
	}

	pd69101->ff_d_gpio = of_get_named_gpio(of_node, ff_d_name, 0);
	if (pd69101->ff_d_gpio < 0) {
		dev_err(&pdev->dev, "GPIO <%s> not found, edge pin not used\n",
			ff_d_name);
		rc = -ENODEV;
	}

	if ((pd69101->ff_clk_gpio > 0)) {
		rc = gpio_request(pd69101->ff_clk_gpio, ff_clk_name);
		if (rc < 0) {
			dev_err(&pdev->dev, "failed to request GPIO %d <%s>\n",
				pd69101->ff_clk_gpio, ff_clk_name);
			goto out_ff_clk_gpio;
		}

		gpio_direction_output(pd69101->ff_clk_gpio, 0);
	}

	if ((pd69101->ff_d_gpio > 0)) {
		rc = gpio_request(pd69101->ff_d_gpio, ff_d_name);
		if (rc < 0) {
			dev_err(&pdev->dev, "failed to request GPIO %d <%s>\n",
				pd69101->ff_d_gpio, ff_d_name);
			goto out_sysfs_fail;
		}

		gpio_direction_output(pd69101->ff_d_gpio, 0);
	}

	rc = sysfs_create_file(&pdev->dev.kobj,
				&dev_attr_reset.attr);

	if (rc < 0) {
		dev_err(&pdev->dev, "sysfs creation failed\n");
		goto out_sysfs_fail;
	}

out_sysfs_fail:
	gpio_free(pd69101->ff_d_gpio);

out_ff_clk_gpio:
	gpio_free(pd69101->ff_clk_gpio);
	return rc;

}

static int pd69101_probe(struct platform_device *pdev)
{
	struct pd69101_data *pd69101;
	struct device_node *of_node = pdev->dev.of_node;
	int rc;
	bool mode_only;

	pd69101 = devm_kzalloc(&pdev->dev, sizeof(*pd69101), GFP_KERNEL);
	if (!pd69101)
		return -ENOMEM;

	pd69101->pdev = pdev;

	pd69101->state = ST_OFF;
	pd69101->time_stamp = 0;
	pd69101->blinking = 0;

	mode_only = of_property_read_bool(of_node, "mode-only");

	if(!mode_only) {
		rc = pd69101_ff_gpio_config(pd69101);
	}

	rc = pd69101_led_gpio_config(pd69101);

	platform_set_drvdata(pdev, pd69101);

	return 0;
}

static int pd69101_remove(struct platform_device *pdev)
{
	struct pd69101_data *pd69101 = dev_get_drvdata(&pdev->dev);

	if (pd69101->ff_clk_gpio >= 0) {
		gpio_direction_output(pd69101->ff_clk_gpio, 1);
		gpio_free(pd69101->ff_clk_gpio);
	}

	if (pd69101->ff_d_gpio >= 0) {
		gpio_free(pd69101->ff_d_gpio);
	}

	return 0;
}

static struct of_device_id of_match_table[] = {
	{ .compatible = "microsemi,pd69101ff", },
	{ .compatible = "microsemi,pd69101", },
	{}
};

static struct platform_driver pd69101_driver = {
	.driver		= {
		.name	= "pd69101ff",
		.of_match_table = of_match_table,
	},
	.probe		= pd69101_probe,
	.remove		= pd69101_remove,
};

module_driver(pd69101_driver, platform_driver_register,
		platform_driver_unregister);

MODULE_DESCRIPTION("PD69101 driver");
MODULE_LICENSE("GPL v2");

