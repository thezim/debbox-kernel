/*
 * ubnt_acl.c: Ubiquiti's ACL abstraction for PHY drivers
 * Copyright 2018 Ubiquiti Networks, Inc.
 */
#include <linux/of_device.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/if_ether.h>
#include <linux/ctype.h>

#include "ubnt_acl.h"

static const char *acl_state_str[] = {
#define X(state) #state,
	ACL_RULE_STATE(X)
#undef X
		NULL
};

/**
 * @brief Clear the ACL linked list
 *
 * @param hw - platform dependent ACL control structure
 */
static void ubnt_acl_flush_sw(struct acl_hw *hw)
{
	acl_entry_t *entry, *entry_temp;

	/**
	 * Drop all entries
	 */
	list_for_each_entry_safe (entry, entry_temp, &hw->acl_table.list, list) {
		list_del(&entry->list);
		kfree(entry);
	}

	hw->acl_table.cnt_active = 0;
	hw->acl_table.cnt_inactive = 0;
	hw->acl_table.idx_last = 0;
}

/**
 * @brief Check if all bytes of mac address are set to zero
 *
 * @param mac mac address to compare
 * @return int 0 - all bytes set to zero
 */
int ubnt_mac_zero_addr(acl_mac_t *mac)
{
	int i;

	if (NULL == mac) {
		return -1;
	}

	for (i = 0; i < sizeof(mac->uc); ++i) {
		if (mac->uc[i]) {
			return -1;
		}
	}
	return 0;
}

/**
 * @brief Compare two MAC addresses
 *
 * @param mac_a mac address a
 * @param mac_b mac address b
 * @return int - 0 mac addresses are both equal to each other
 */
static int ubnt_parse_acl_rule_mac_match(acl_mac_t *mac_a, acl_mac_t *mac_b)
{
	return memcmp(mac_a->uc, mac_b->uc, sizeof(mac_b->uc));
}

/**
 * @brief Compare two rules
 *
 * @param mac_a entry a
 * @param mac_bentry b
 * @return int - 0 entries are both equal to each other
 */
static int ubnt_acl_rule_match(acl_entry_t *entry_a, acl_entry_t *entry_b)
{
	int rc = -1;

	if (NULL == entry_a || NULL == entry_b) {
		return -EINVAL;
	}

	if (entry_a->type == entry_b->type) {
		switch (entry_a->type) {
		case ACL_RULE_PORT_REDIRECTION:
			if (entry_a->port_dst == entry_b->port_dst) {
				rc = ubnt_parse_acl_rule_mac_match(&entry_a->mac_sa,
								   &entry_b->mac_sa);
				rc |= ubnt_parse_acl_rule_mac_match(&entry_a->mac_da,
								    &entry_b->mac_da);
			}
			break;
		case ACL_RULE_VLAN_ASSIGNMENT:
			rc = !(entry_a->svid == entry_b->svid);
			break;
		case ACL_RULE_ETHVLAN:
			if (entry_a->port_dst == entry_b->port_dst &&
			    entry_a->port_src == entry_b->port_src &&
			    entry_a->ether_type == entry_b->ether_type)
				rc = 0;
			break;
		default:
			break;
		}
	}
	return rc;
}

/**
 * @brief Update rule
 *
 * @param entry_old the old entry
 * @param entry_new the new one
 * @return int - error from errno.h, 0 on success
 */
static int ubnt_acl_rule_update(acl_entry_t *entry_old, acl_entry_t *entry_new)
{
	if (NULL == entry_old || NULL == entry_new) {
		return -EINVAL;
	}

	switch (entry_old->type) {
	case ACL_RULE_PORT_REDIRECTION:
		entry_old->port_src |= entry_new->port_src;
		break;
	case ACL_RULE_VLAN_ASSIGNMENT:
		entry_old->ether_type = entry_new->ether_type;
		break;
	case ACL_RULE_ETHVLAN:
		entry_old->ether_type = entry_new->ether_type;
		entry_old->force_vtu = entry_new->force_vtu;
		entry_old->vlan_src = entry_new->vlan_src;
		entry_old->vlan_dst = entry_new->vlan_dst;
		break;
	default:
		break;
	}

	if (ACL_RULE_ACTIVE == entry_old->state) {
		entry_old->state = ACL_RULE_MODIFIED;
	}

	return 0;
}

/**
 * @brief Find ACL rule
 *
 * @param hw - platform dependent ACL control structure
 * @param entry_in - entry to find - key should be filled
 * @return int - error from errno.h, 0 on success
 */
int ubnt_acl_rule_get(struct acl_hw *hw, acl_entry_t *entry_in)
{
	acl_entry_t *entry = NULL, *entry_temp = NULL;

	if (!entry_in) {
		return -EINVAL;
	}

	list_for_each_entry_safe (entry, entry_temp, &hw->acl_table.list, list) {
		if(entry->state != ACL_RULE_DELETED) {
			if (!ubnt_acl_rule_match(entry_in, entry)) {
				*entry_in = *entry;
				return 0;
			}
		}
	}
	return -EINVAL;
}

/**
 * @brief Add rule to the ACL's linked list
 *
 * @param hw - platform dependent ACL control structure
 * @param entry_new  - new ACL entry
 * @return int - error from errno.h, 0 on success
 */
static int ubnt_acl_rule_add(struct acl_hw *hw, acl_entry_t *entry_new)
{
	acl_entry_t *entry, *entry_temp;

	list_for_each_entry_safe (entry, entry_temp, &hw->acl_table.list, list) {
		if (!ubnt_acl_rule_match(entry, entry_new)) {
			if (!ubnt_acl_rule_update(entry, entry_new)) {
				goto done;
			} else {
				return -EINVAL;
			}
		}
	}

	if (ACL_TOTAL_CNT(hw->acl_table) >= hw->ops->max_entries) {
		return -ENOSPC;
	}

	entry = kzalloc(sizeof(*entry), GFP_KERNEL);
	if (NULL == entry) {
		return -ENOMEM;
	}

	memcpy(entry, entry_new, sizeof(*entry));
	entry->state = ACL_RULE_INACTIVE;
	entry->idx = -1;
	INIT_LIST_HEAD(&entry->list);
	list_add_tail(&(entry->list), &(hw->acl_table.list));
	++hw->acl_table.cnt_inactive;

done:
	return 0;
}

/**
 * @brief Remove rule from the ACL's linked list
 *
 * @param hw - platform dependent ACL control structure
 * @param entry_out  - the ACL entry to be modified/deleted
 * @return int - error from errno.h, 0 on success
 */
static int ubnt_acl_rule_del(struct acl_hw *hw, acl_entry_t *entry_out)
{
	acl_entry_t *entry, *entry_temp;
	bool modified = false;

	list_for_each_entry_safe (entry, entry_temp, &hw->acl_table.list, list) {
		if (!ubnt_acl_rule_match(entry_out, entry)) {
			switch (entry->type) {
			case ACL_RULE_PORT_REDIRECTION:
				entry->port_src &= ~(entry_out->port_src);
				modified = !!(entry->port_src);
				break;
			case ACL_RULE_ETHVLAN:
			case ACL_RULE_VLAN_ASSIGNMENT:
				modified = false;
				break;
			default:
				return 0;
			}

			if (modified) {
				if (ACL_RULE_ACTIVE == entry->state) {
					entry->state = ACL_RULE_MODIFIED;
				}
			} else {
				if (ACL_RULE_INACTIVE == entry->state) {
					list_del(&entry->list);
					kfree(entry);
					--hw->acl_table.cnt_inactive;
				} else {
					entry->state = ACL_RULE_DELETED;
				}
			}
		}
	}
	return 0;
}

/**
 * @brief Parser macros
 */
#define KEY_WORDS(X)            \
	X(CMD_ADD, "add")           \
	X(CMD_DEL, "del")           \
	X(MAC_SRC, "mac_src")       \
	X(MAC_DST, "mac_dst")       \
	X(PORT_DST, "port_dst")     \
	X(PORTS_SRC, "ports_src")   \
	X(ETHER_TYPE, "ether_type") \
	X(SVID, "svid")             \
	X(VLAN_SRC, "vlan_src")     \
	X(VLAN_DST, "vlan_dst")     \
	X(FORCE_VTU, "force_vtu")


#define KEY_OP(X) \
	X(CMD)    \
	X(KEY)    \
	X(VAL)

enum {
#define X(name) ACL_OP_##name,
	KEY_OP(X)
#undef X
		ACL_OP_CNT
};

enum {
#define X(name, key) ACL_KEY_##name,
	KEY_WORDS(X)
#undef X
		ACL_KEY_CNT
};

static const char *acl_keywords[] = {
#define X(name, key) key,
	KEY_WORDS(X)
#undef X
		NULL
};

/**
 * @brief Parse the MAC address and store result in `mac`.
 *
 * @param rule - mac address of ACL rule.
 * @param mac - mac address of ACL mac.
 * @return char* - string after value or NULL on error.
 * @note single quote tolerant.
 */
static const char *acl_parse_mac(const char *rule, u8 mac[6])
{
	while (isblank(*rule) || *rule == '\'')
		rule++;

	if (*rule == '*') // in original implementation `*` any mac, no rule
		;
	else if (sscanf(rule, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
			&mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5])
			== ETH_ALEN)
		;
	else
		return NULL;

	while (isgraph(*rule))
		rule++;
	return rule;
}

const char *acl_parse_value(const char *rule, uint16_t *value)
{
	while (isblank(*rule) || *rule == '\'')
		rule++;

	if (sscanf(rule, "0x%hX", value) == 1)
		;
	else if (sscanf(rule, "%hu", value) == 1)
		;
	else
		return NULL;

	while (isgraph(*rule))
		rule++;
	return rule;
}

const char *acl_parse_bitmask(const char *rule, u32 *mask)
{
	u32 value;

	*mask = 0;
	while (1) {
		while (isblank(*rule) || *rule == '\'')
			rule++;
		if (*rule == '\0')
			break;

		if (sscanf(rule, "%u", &value) == 0)
			break;

		// shift boundary check
		if (value > (8 * sizeof(value) - 1))
			return NULL;

		*mask |= (1 << value);

		while (isgraph(*rule))
			rule++;
	}
	if (*mask == 0)
		return NULL; // no successful conversions
	return rule;
}

/**
 * @brief Parse ACL rule
 *
 * @param hw - platform dependent ACL control structure
 * @param rule - ACL rule
 * @param rule_type - type of ACL rule
 * @return int - error from errno.h, 0 on success
 */
int ubnt_acl_rule_process(struct acl_hw *hw, const char *rule, acl_entry_type_t rule_type)
{
	int cmd, i;
	const char *ptr;
	acl_entry_t entry_out;
	char key_hit[ACL_KEY_CNT] = { 0 };

	if (NULL == hw)
		return -EINVAL;

	memset(&entry_out, 0, sizeof(entry_out));
	entry_out.type = rule_type;

	/**
	 * Format :
	 * cmd <key> '<value>'
	 * cmd <key>  <value>
	 * ACL_RULE_PORT_REDIRECTION
	 *		"add mac_src '*' mac_dst '*' ports_src '1 2 3 4' port_dst '0'"
	 *		"add mac_src '6A:FC:9E:83:40:68' mac_dst '*' ports_src '0 2' port_dst '1'"
	 *		"del mac_src '*' ports_src '1 2 3 4' mac_dst '*' port_dst '0'"
	 *		"del mac_src '6A:FC:9E:83:40:68' mac_dst '*' ports_src '0 2' port_dst '1'"
	 * ACL_RULE_VLAN_ASSIGNMENT
	 * 		"add svid '4094' ether_type '0x88cc'"
	 *		"del svid '4094'"
	 * ACL_RULE_ETHVLAN
	 *		"add ports_src '0' port_dst '1' vlan_dst '1' vlan_src '4091' force_vtu '1'"
	 *		"add ports_src '1' port_dst '0' vlan_dst '4091' ether_type '0x88CC'"
	 *		"del ports_src '0' port_dst '1'"
	 */
	enum { ACL_RULE_CMD_ADD = 0, ACL_RULE_CMD_DEL, ACL_RULE_CMD_UNKNOWN };

	while (isblank(*rule))
		rule++;

	// check CMD type (Add, del...)
	if (!memcmp(rule, acl_keywords[ACL_KEY_CMD_ADD],
		    strlen(acl_keywords[ACL_KEY_CMD_ADD]))) {
		// Add rule
		rule += strlen(acl_keywords[ACL_KEY_CMD_ADD]);
		cmd = ACL_RULE_CMD_ADD;
	} else if (!memcmp(rule, acl_keywords[ACL_KEY_CMD_DEL],
			   strlen(acl_keywords[ACL_KEY_CMD_DEL]))) {
		// Del rule
		rule += strlen(acl_keywords[ACL_KEY_CMD_DEL]);
		cmd = ACL_RULE_CMD_DEL;
	} else {
		pr_err("[ACL] unknown cmd: '%s'\n", rule);
		return -EINVAL;
	}

	while (isblank(*rule))
		rule++;
	while (*rule) {
		// check on 0
		for (i = ACL_KEY_MAC_SRC; i < ACL_KEY_CNT; i++) {
			if (!memcmp(rule, acl_keywords[i], strlen(acl_keywords[i]))) {
				rule += strlen(acl_keywords[i]);
				break;
			}
		}

		switch (i) {
		case ACL_KEY_MAC_SRC:
			ptr = acl_parse_mac(rule, &entry_out.mac_sa.uc[0]);
			break;
		case ACL_KEY_MAC_DST:
			ptr = acl_parse_mac(rule, &entry_out.mac_da.uc[0]);
			break;
		case ACL_KEY_PORT_DST:
			ptr = acl_parse_bitmask(rule, &entry_out.port_dst);
			break;
		case ACL_KEY_PORTS_SRC:
			ptr = acl_parse_bitmask(rule, &entry_out.port_src);
			break;
		case ACL_KEY_SVID:
			ptr = acl_parse_value(rule, &entry_out.svid);
			break;
		case ACL_KEY_ETHER_TYPE:
			ptr = acl_parse_value(rule, &entry_out.ether_type);
			break;
		case ACL_KEY_VLAN_SRC:
			ptr = acl_parse_value(rule, &entry_out.vlan_src);
			break;
		case ACL_KEY_VLAN_DST:
			ptr = acl_parse_value(rule, &entry_out.vlan_dst);
			break;
		case ACL_KEY_FORCE_VTU:
			ptr = acl_parse_value(rule, &entry_out.force_vtu);
			break;
		case ACL_KEY_CNT:
		default:
			pr_err("[ACL] unknown rule: '%s'\n", rule);
			return -EINVAL;
		}
		if (ptr == NULL) {
			pr_err("[ACL] Failed to parse val: '%s'\n", rule);
			return -EINVAL;
		}
		key_hit[i] = 1;
		rule = ptr;
		// next parameter
		while (isblank(*rule))
			rule++;
	}

	/* Presence validation */
	switch (rule_type) {
	case ACL_RULE_PORT_REDIRECTION:
		if (!key_hit[ACL_KEY_MAC_SRC] || !key_hit[ACL_KEY_MAC_DST] ||
		    !key_hit[ACL_KEY_PORT_DST] || !key_hit[ACL_KEY_PORTS_SRC]) {
			return -EINVAL;
		}
		break;
	case ACL_RULE_VLAN_ASSIGNMENT:
		if (!key_hit[ACL_KEY_ETHER_TYPE] ||
		    (ACL_RULE_CMD_ADD == cmd && !key_hit[ACL_KEY_SVID])) {
			return -EINVAL;
		}
		break;
	case ACL_RULE_ETHVLAN:
		if (!key_hit[ACL_KEY_PORT_DST] ||
		    !key_hit[ACL_KEY_PORTS_SRC]) {
			return -EINVAL;
		}
		break;
	default:
		pr_err("[ACL] unknown rule type: '%d'\n", rule_type);
		return -EINVAL;
	}

	switch (cmd) {
	case (ACL_RULE_CMD_ADD):
		return ubnt_acl_rule_add(hw, &entry_out);
	case (ACL_RULE_CMD_DEL):
		return ubnt_acl_rule_del(hw, &entry_out);
	default:
		BUG(); // cmd check is above
	}
	return 0;
}

/**
 * @brief Get ACL table
 *
 * @param hw - platform dependent ACL control structure
 */
int ubnt_acl_get_acl_table(struct acl_hw *hw)
{
	int i;
	acl_entry_t *entry, *entry_temp;

	/** Output string */
	char *str;
	uint32_t str_len = 0;
	uint32_t str_max;

	if (NULL == hw) {
		return -EINVAL;
	}

	if (NULL == hw->str) {
		return -EINVAL;
	}

	str = hw->str;
	str_max = hw->str_max;
	str[0] = '\0';
	hw->str_len = 0;

	BUF_APPEND_PRINTF("ACL Enabled: %d\n", hw->acl_table.enable);

	if (ACL_TOTAL_CNT(hw->acl_table)) {
		list_for_each_entry_safe (entry, entry_temp, &hw->acl_table.list, list) {
			if (entry->state == ACL_RULE_INACTIVE) {
					BUF_APPEND_PRINTF("ID: <Unassigned>\n");
			} else {
					BUF_APPEND_PRINTF("ID <%d>\n", entry->idx);
			}

			BUF_APPEND_PRINTF("RULE TYPE: : %d\n", entry->type);

			switch(entry->type) {
				case ACL_RULE_PORT_REDIRECTION:
					BUF_APPEND_PRINTF("MAC_SRC: ");
					if (ubnt_mac_zero_addr(&entry->mac_sa)) {
						BUF_APPEND_PRINTF("%02x:%02x:%02x:%02x:%02x:%02x\n",
								entry->mac_sa.uc[0], entry->mac_sa.uc[1],
								entry->mac_sa.uc[2], entry->mac_sa.uc[3],
								entry->mac_sa.uc[4], entry->mac_sa.uc[5]);
					} else {
						BUF_APPEND_PRINTF("*\n");
					}

					BUF_APPEND_PRINTF("MAC_DST: ");
					if (ubnt_mac_zero_addr(&entry->mac_da)) {
						BUF_APPEND_PRINTF("%02x:%02x:%02x:%02x:%02x:%02x\n",
								entry->mac_da.uc[0], entry->mac_da.uc[1],
								entry->mac_da.uc[2], entry->mac_da.uc[3],
								entry->mac_da.uc[4], entry->mac_da.uc[5]);
					} else {
						BUF_APPEND_PRINTF("*\n");
					}

					BUF_APPEND_PRINTF("PORTS_SRC: ");

					for (i = 0; i < hw->max_ports; ++i) {
						if (!(entry->port_src & (1 << i))) {
							continue;
						}
						BUF_APPEND_PRINTF("%d ", i);
					}

					BUF_APPEND_PRINTF("\nPORT_DST: ");
					for (i = 0; i < hw->max_ports; ++i) {
						if (!(entry->port_dst & (1 << i))) {
							continue;
						}
						BUF_APPEND_PRINTF("%d ", i);
					}
					break;
				case ACL_RULE_VLAN_ASSIGNMENT:
					BUF_APPEND_PRINTF("ETHER_TYPE: 0x%04X\n", entry->ether_type);
					BUF_APPEND_PRINTF("SVID: %d\n", entry->svid);
					break;
				case ACL_RULE_ETHVLAN:
					BUF_APPEND_PRINTF("PORTS_SRC: ");
					for (i = 0; i < hw->max_ports; ++i) {
						if (!(entry->port_src & (1 << i))) {
							continue;
						}
						BUF_APPEND_PRINTF("%d ", i);
					}
					BUF_APPEND_PRINTF("\nPORT_DST: ");
					for (i = 0; i < hw->max_ports; ++i) {
						if (!(entry->port_dst & (1 << i))) {
							continue;
						}
						BUF_APPEND_PRINTF("%d ", i);
					}
					BUF_APPEND_PRINTF("\n");
					if (entry->ether_type) {
						BUF_APPEND_PRINTF("ETHER_TYPE: 0x%04X\n", entry->ether_type);
					}
					if (entry->vlan_dst) {
						BUF_APPEND_PRINTF("VLAN_DST: %d\n", entry->vlan_dst);
					}
					if (entry->vlan_src) {
						BUF_APPEND_PRINTF("VLAN_SRC: %d\n", entry->vlan_src);
					}
					if (entry->force_vtu) {
						BUF_APPEND_PRINTF("FORCE_VTU\n");
					}
					break;
				default:
					BUF_APPEND_PRINTF("UNKNOWN TYPE: 0x%X\n", entry->type);
					break;
			}
			BUF_APPEND_PRINTF("\nSTATE: %s\n\n", acl_state_str[entry->state]);
		}
	}

	hw->str_len = str_len;

	return 0;
error:
	return -ENOMEM;
}

/**
 * @brief Dealloc the ACL control structures
 *
 * @param hw - platform dependent ACL control structure
 */
void ubnt_acl_destroy(struct acl_hw *hw)
{
	if (NULL != hw) {
		ubnt_acl_flush_sw(hw);
		if (hw->str) {
			kfree(hw->str);
		}
		hw->str_len = 0;
		hw->str_max = 0;
		if (NULL != hw->ops && NULL != hw->ops->destroy) {
			hw->ops->destroy(hw);
		}
	}
}

/**
 * @brief Init the ACL
 *
 * @param hw - platform dependent ACL control structure
 * @return int - error from errno.h, 0 on success
 */
int ubnt_acl_init(struct acl_hw *hw)
{
	int rc = 0;

	if (NULL == hw) {
		return -EINVAL;
	}

	if (NULL == hw->ops) {
		return -EINVAL;
	}

	if (NULL == hw->ops->flush || NULL == hw->ops->init || NULL == hw->ops->enable ||
	    NULL == hw->ops->rule_set || NULL == hw->ops->rule_delete ||
	    NULL == hw->ops->rule_sw_to_hw || NULL == hw->ops->alloc || NULL == hw->ops->destroy) {
		return -EINVAL;
	}

	/* Alloc platform dependent */
	rc = hw->ops->alloc(hw);
	if (rc) {
		return rc;
	}

	if (NULL == hw->data) {
		return -EINVAL;
	}

	/** Buffer allocation */
	hw->str_len = 0;
	hw->str_max = ACL_BUFF_SIZE(hw->ops->max_entries);
	hw->str = kzalloc(hw->str_max, GFP_KERNEL);
	if (NULL == hw->str) {
		ubnt_acl_destroy(hw);
		return -ENOMEM;
	}

	/**
	 * ACL list init
	 */
	INIT_LIST_HEAD(&hw->acl_table.list);
	ubnt_acl_flush_sw(hw);
	hw->acl_table.enable = 0;

	/**
	 * Init HW ACL module
	 */
	rc = hw->ops->init(hw);
	if (rc) {
		ubnt_acl_destroy(hw);
		return rc;
	}

	/**
	 * Flush ACL in HW
	 */
	rc = hw->ops->flush(hw);
	if (rc) {
		ubnt_acl_destroy(hw);
		return rc;
	}

	return 0;
}

/**
 * @brief Retrive ACL status
 *
 * @param hw - platform dependent ACL control structure
 * @param enable ACL status
 * @return int - error from errno.h, 0 on success
 */
int ubnt_acl_enable_get(struct acl_hw *hw, int *enable)
{
	if (NULL == hw || NULL == enable) {
		return -EINVAL;
	}
	*enable = hw->acl_table.enable;
	return 0;
}

/**
 * @brief Set ACL status
 *
 * @param hw - platform dependent ACL control structure
 * @param enable ACL status
 * @return int - error from errno.h, 0 on success
 */
int ubnt_acl_enable_set(struct acl_hw *hw, int enable)
{
	if (NULL == hw) {
		return -EINVAL;
	}
	hw->acl_table.enable = !!enable;
	return 0;
}

/**
 * @brief Flush the switch hw's ACL & clear the ACL linked list
 *
 * @param hw - platform dependent ACL control structure
 * @return int - error from errno.h, 0 on success
 */
int ubnt_acl_flush(struct acl_hw *hw)
{
	int rc = 0;

	if (NULL == hw) {
		return -EINVAL;
	}

	if (NULL == hw->ops) {
		return -EINVAL;
	}

	if (NULL == hw->ops->flush) {
		return -EINVAL;
	}

	ubnt_acl_flush_sw(hw);
	/**
	 * Flush ACL in HW
	 */
	rc = hw->ops->flush(hw);
	if (rc) {
		return rc;
	}
	return 0;
}

/**
 * @brief Load switch hw's ACL list to linked list and sync
 *
 * @param hw - platform dependent ACL control structure
 * @return int - error from errno.h, 0 on success
 */
int ubnt_acl_preload(struct acl_hw *hw)
{
	int i = 0, rc = 0;
	acl_entry_t entry_sw;

	if (NULL == hw) {
		return -EINVAL;
	}

	if (NULL == hw->ops) {
		return -EINVAL;
	}

	if (NULL == hw->ops->flush || NULL == hw->ops->rule_hw_to_sw || NULL == hw->ops->rule_get) {
		return -EINVAL;
	}

	ubnt_acl_flush_sw(hw);

	for (i = 0; i < hw->ops->max_entries; ++i) {
		rc = hw->ops->rule_get(hw, i, hw->data);
		if (!rc) {
			if (!hw->ops->rule_hw_to_sw(hw, hw->data, &entry_sw)) {
				ubnt_acl_rule_add(hw, &entry_sw);
			} else {
				/* Ignore unsupported ACL rules*/
				continue;
			}
		} else {
			goto exit;
		}
	}

	/**
	 * Flush ACL in HW
	 */
	rc = hw->ops->flush(hw);
	if (rc) {
		return rc;
	}
	ubnt_acl_sync(hw);

exit:
	return rc;
}

/**
 * @brief Flush HW ACL, clear counters, clear idx_last, remove deleted rules from
 * the linked list and set all other rules to INACTIVE state.
 *
 * @param hw - platform dependent ACL control structure
 * @return int - error from errno.h, 0 on success
 */
static void ubnt_acl_sync_squash(struct acl_hw *hw)
{
	acl_entry_t *entry, *entry_temp;

	if (NULL == hw) {
		return;
	}

	if (NULL == hw->ops) {
		return;
	}

	if (NULL == hw->ops->flush) {
		return;
	}

	if ((hw->acl_table.idx_last + hw->acl_table.cnt_inactive) < hw->ops->max_entries) {
		return;
	}
	/** Flush all ACL entries from HW */
	hw->ops->flush(hw);
	hw->acl_table.idx_last = 0;
	hw->acl_table.cnt_inactive = 0;
	hw->acl_table.cnt_active = 0;

	list_for_each_entry_safe (entry, entry_temp, &hw->acl_table.list, list) {
		if (ACL_RULE_DELETED == entry->state) {
			/**
			 * Ged rid of deleted rules, they are no longer exist in HW
			 */
			list_del(&entry->list);
			kfree(entry);
		} else {
			/**
			 * Set all other active rules to ACL_RULE_INACTIVE state.
			 * Sync will cause that new IDs will be assign to each of
			 * the inactive rule.
			 */
			entry->state = ACL_RULE_INACTIVE;
			++hw->acl_table.cnt_inactive;
		}
	}
}

/**
 * @brief Sync the ACL linked list with switch hw's linked list
 *
 * @param hw - platform dependent ACL control structure
 * @return int - error from errno.h, 0 on success
 */
int ubnt_acl_sync(struct acl_hw *hw)
{
	acl_entry_t *entry, *entry_temp;
	int rc = 0;

	if (NULL == hw) {
		return -EINVAL;
	}

	if (NULL == hw->ops) {
		return -EINVAL;
	}

	if (NULL == hw->ops->rule_delete || NULL == hw->ops->rule_sw_to_hw ||
	    NULL == hw->ops->rule_set || NULL == hw->ops->enable) {
		return -EINVAL;
	}

	/* Squash entries if there is no place in ACL. Keep the order of the rules */
	ubnt_acl_sync_squash(hw);

	list_for_each_entry_safe (entry, entry_temp, &hw->acl_table.list, list) {
		switch (entry->state) {
		case ACL_RULE_DELETED:
			/**
			 * Delete the rule from switch hw
			 */
			if (!(rc=hw->ops->rule_delete(hw, entry->idx))) {
				list_del(&entry->list);
				kfree(entry);
				--hw->acl_table.cnt_active;
			} else {
				goto exit;
			}
			break;
		case ACL_RULE_INACTIVE:
			entry->idx = hw->acl_table.idx_last;
			/* Intentional fall through to modify op */
		case ACL_RULE_MODIFIED:

			if ((rc = hw->ops->rule_sw_to_hw(hw, entry, hw->data))) {
				goto exit;
			}

			if (ACL_RULE_MODIFIED == entry->state) {
				if ((rc = hw->ops->rule_delete(hw, entry->idx))) {
					goto exit;
				}
			}

			/**
			 * Write the rule to switch hw
			 */
			if (!(rc = hw->ops->rule_set(hw, entry->idx, hw->data))) {
				if (ACL_RULE_INACTIVE == entry->state) {
					++hw->acl_table.idx_last;
					--hw->acl_table.cnt_inactive;
					++hw->acl_table.cnt_active;
				}
				entry->state = ACL_RULE_ACTIVE;
			} else {
				goto exit;
			}
			break;

		case ACL_RULE_ACTIVE:
			continue;
			break;

		default:
			rc = -EINVAL;
			goto exit;
		}
	}

	rc = hw->ops->enable(hw, hw->acl_table.enable);
exit:
	return rc;
}
