/*
 * Copyright 2016, Amazon.com, Inc. or its affiliates. All Rights Reserved
 */

/**
 *  @{
 * @file   al_hal_serdes_avg_internal.h
 *
 * @brief Avago SerDes internal definitions
 *
 */

#ifndef __AL_HAL_SERDES_AVG_INTERNAL_H__
#define __AL_HAL_SERDES_AVG_INTERNAL_H__

#include "al_hal_plat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* o_core_status bits */
#define O_CORE_STATUS_KR_TRAINING_FAILURE	AL_BIT(0)
#define O_CORE_STATUS_KR_TRAINING_IN_PROGRESS	AL_BIT(1)
#define O_CORE_STATUS_RX_TRAINED		AL_BIT(2)
#define O_CORE_STATUS_RX_SIGNAL_OK		AL_BIT(4)

/* Core interrupts */
#define CORE_INT_REVISION_GET_REG_NUM		0

#define CORE_INT_SRDS_EN_REG_NUM		1
#define CORE_INT_SRDS_EN_DIS			0
#define CORE_INT_SRDS_EN_TX_EN			AL_BIT(0)
#define CORE_INT_SRDS_EN_RX_EN			AL_BIT(1)
#define CORE_INT_SRDS_EN_OUT_EN			AL_BIT(2)

#define CORE_INT_SRDS_PLL_RECAL			0x11
#define CORE_INT_SRDS_PLL_RECAL_TX_PLL_RECAL	AL_BIT(0)
#define CORE_INT_SRDS_PLL_RECAL_RX_PLL_RECAL	AL_BIT(1)

#define CORE_INT_PRBS_CTRL_REG_NUM		0x02
#define CORE_INT_PRBS_CTRL_PATTERN_MASK		AL_BIT_FIELD(2, 0)
#define CORE_INT_PRBS_CTRL_PATTERN_SHIFT	0
#define CORE_INT_PRBS_CTRL_RESEED_ON_ERR	AL_BIT(4)
#define CORE_INT_PRBS_CTRL_AUTO_SEED		AL_BIT(5)
#define CORE_INT_PRBS_CTRL_STOP_ON_ERR		AL_BIT(7)
#define CORE_INT_PRBS_CTRL_TX_EN		AL_BIT(8)
#define CORE_INT_PRBS_CTRL_RX_EN		AL_BIT(9)
#define CORE_INT_PRBS_CTRL_TX_DIS_VAL		0x1ff
#define CORE_INT_PRBS_CTRL_RX_DIS_VAL		0x2ff
#define CORE_INT_PRBS_CTRL_TX_RX_DIS_VAL	0x3ff

#define CORE_INT_CMP_CTRL_REG_NUM		0x03
#define CORE_INT_CMP_CTRL_CMP_EN		AL_BIT(0)
#define CORE_INT_CMP_CTRL_CMP_SUM		AL_BIT(1)
#define CORE_INT_CMP_CTRL_SRC_DATA_VAL_RX	(0 << 4)
#define CORE_INT_CMP_CTRL_CMP_DATA_VAL_USR_PRBS	(2 << 8)
#define CORE_INT_CMP_CTRL_CMP_DATA_VAL_TX_PRBS	(4 << 8)
#define CORE_INT_CMP_CTRL_CMP_DATA_VAL_TX_DATA	(3 << 8)

#define CORE_INT_PMD_CTRL_REG_NUM		0x04
#define CORE_INT_PMD_CTRL_RESTART_TRAINING	0
#define CORE_INT_PMD_CTRL_LAUNCH_PMD_TRAIN	2

#define CORE_INT_TX_BIT_RATE_REG_NUM		5
#define CORE_INT_TX_BIT_RATE_RATIO_MASK		AL_BIT_FIELD(7, 0)
#define CORE_INT_TX_BIT_RATE_RATIO_SHIFT	0
#define CORE_INT_TX_BIT_RATE_RATE_MASK		AL_BIT_FIELD(10, 8)
#define CORE_INT_TX_BIT_RATE_RATE_SHIFT		8
#define CORE_INT_TX_BIT_RATE_RATE_VAL_1_1	0
#define CORE_INT_TX_BIT_RATE_RATE_VAL_1_2	1
#define CORE_INT_TX_BIT_RATE_RATE_VAL_1_4	2
#define CORE_INT_TX_BIT_RATE_RATE_VAL_1_8	3
#define CORE_INT_TX_BIT_RATE_RATE_VAL_1_16	4
#define CORE_INT_TX_BIT_RATE_REFCLK_SYNC_MASTER	AL_BIT(12)
#define CORE_INT_TX_BIT_RATE_APPLY_BOTH_TX_RX	AL_BIT(15)

#define CORE_INT_RX_BIT_RATE_REG_NUM		6
#define CORE_INT_RX_BIT_RATE_RATIO_MASK		AL_BIT_FIELD(7, 0)
#define CORE_INT_RX_BIT_RATE_RATIO_SHIFT	0
#define CORE_INT_RX_BIT_RATE_RATE_MASK		AL_BIT_FIELD(10, 8)
#define CORE_INT_RX_BIT_RATE_RATE_SHIFT		8
#define CORE_INT_RX_BIT_RATE_RATE_VAL_1_1	0
#define CORE_INT_RX_BIT_RATE_RATE_VAL_1_2	1
#define CORE_INT_RX_BIT_RATE_RATE_VAL_1_4	2
#define CORE_INT_RX_BIT_RATE_RATE_VAL_1_8	3
#define CORE_INT_RX_BIT_RATE_RATE_VAL_1_16	4
#define CORE_INT_RX_BIT_RATE_APPLY_BOTH_TX_RX	AL_BIT(15)

#define CORE_INT_DFE_CTRL_REG_NUM		0x0a
#define CORE_INT_DFE_CTRL_RUN_ICAL		1
#define CORE_INT_DFE_CTRL_RUN_PCAL_SINGLE	2
#define CORE_INT_DFE_CTRL_RUN_PCAL_ADAPTIVE	6
#define CORE_INT_DFE_CTRL_FIXED_DC		AL_BIT(7)

#define CORE_INT_TX_PHASE_CAL_REG_NUM		0x0b
#define CORE_INT_TX_PHASE_CAL_EN		AL_BIT(0)

#define CORE_INT_RX_PHASE_SLIP_REG_NUM			0x0e
#define CORE_INT_RX_PHASE_SLIP_RX_CLK_MASK		AL_BIT_FIELD(4, 0)
#define CORE_INT_RX_PHASE_SLIP_RX_CLK_SHIFT		0
#define CORE_INT_RX_PHASE_SLIP_RX_FIFO_CLK_MASK		AL_BIT_FIELD(13, 8)
#define CORE_INT_RX_PHASE_SLIP_RX_FIFO_CLK_SHIFT	8
#define CORE_INT_RX_PHASE_SLIP_ON_RX_ENABLE		AL_BIT(15)

#define CORE_INT_WIDTH_MODE_REG_NUM		0x14
#define CORE_INT_WIDTH_MODE_TX_WIDTH_MASK	AL_BIT_FIELD(2, 0)
#define CORE_INT_WIDTH_MODE_TX_WIDTH_SHIFT	0
#define CORE_INT_WIDTH_MODE_TX_WIDTH_VAL_10	0
#define CORE_INT_WIDTH_MODE_TX_WIDTH_VAL_20	2
#define CORE_INT_WIDTH_MODE_TX_WIDTH_VAL_32	5
#define CORE_INT_WIDTH_MODE_TX_WIDTH_VAL_40	3
#define CORE_INT_WIDTH_MODE_RX_WIDTH_MASK	AL_BIT_FIELD(6, 4)
#define CORE_INT_WIDTH_MODE_RX_WIDTH_SHIFT	4
#define CORE_INT_WIDTH_MODE_RX_WIDTH_VAL_10	0
#define CORE_INT_WIDTH_MODE_RX_WIDTH_VAL_20	2
#define CORE_INT_WIDTH_MODE_RX_WIDTH_VAL_32	5
#define CORE_INT_WIDTH_MODE_RX_WIDTH_VAL_40	3

#define CORE_INT_TX_EQ_REG_NUM			0x15
#define CORE_INT_TX_EQ_TAP_SELECTION_SHIFT	12
#define CORE_INT_TX_EQ_TAP_SELECTION_SET(val)	((val) << CORE_INT_TX_EQ_TAP_SELECTION_SHIFT)
#define CORE_INT_TX_EQ_TAP_SELECTION_PRE1_SET(val)	(CORE_INT_TX_EQ_TAP_SELECTION_SET(0x0) | \
							(val))
#define CORE_INT_TX_EQ_TAP_SELECTION_ATTEN_SET(val)	(CORE_INT_TX_EQ_TAP_SELECTION_SET(0x4) | \
							(val))
#define CORE_INT_TX_EQ_TAP_SELECTION_POST1_SET(val)	(CORE_INT_TX_EQ_TAP_SELECTION_SET(0x8) | \
							(val))

#define CORE_INT_CMP_CTRL_TMR_LO_REG_NUM	0x103

#define CORE_INT_CMP_CTRL_TMR_HI_REG_NUM	0x203

#define CORE_INT_BIG_REG_SEL_REG_NUM		0x18

#define CORE_INT_BIG_REG_WRITE_REG_NUM		0x19

#define CORE_INT_BIG_REG_READ_REG_NUM		0x1a

#define CORE_INT_ERR_INJECT_REG_NUM		0x1B

#define CORE_INT_ERR_STATUS_REG_NUM		0x1E
#define CORE_INT_ERR_STATUS_ERR			AL_BIT(4)
#define CORE_INT_ERR_STATUS_ERR_OVFLW		AL_BIT(5)

#define CORE_INT_IDLE_DETECTOR_REG_NUM			0x20
#define CORE_INT_IDLE_DETECTOR_RX_IDLE_DETECTOR_EN	AL_BIT(5)
#define CORE_INT_IDLE_DETECTOR_THR_SET(thr)	(((thr) << 8) | 0x40)
#define CORE_INT_IDLE_DETECTOR_PCIE_DEFAULT	CORE_INT_IDLE_DETECTOR_THR_SET(0x64)
#define CORE_INT_IDLE_DETECTOR_ETH_SD_DEFAULT	CORE_INT_IDLE_DETECTOR_THR_SET(3)

#define CORE_INT_SET_RX_EQ_REG_NUM		0x26
#define CORE_INT_SET_RX_EQ_PCIE_RUN_ICAL_DFLT	0x5202
#define CORE_INT_SET_RX_EQ_RUN_PCAL_SINGLE	0x5b00
#define CORE_INT_SET_RX_EQ_AUTO_PCAL_DIS	0x5b01
#define CORE_INT_SET_RX_EQ_PCIE_CTLE_HF_SEED	0x000f
#define CORE_INT_SET_RX_EQ_PCIE_CTLE_LF_SEED	0x0100
#define CORE_INT_SET_RX_EQ_PCIE_CTLE_DC_SEED	0x0232
#define CORE_INT_SET_RX_EQ_CTLE_HF(val)		(0x2000 | val)
#define CORE_INT_SET_RX_EQ_CTLE_LF(val)		(0x2100 | val)
#define CORE_INT_SET_RX_EQ_CTLE_BW(val)		(0x2300 | val)
#define CORE_INT_SET_EX_EQ_PCIE_CTLE_DC		0x2278

#define CORE_INT_TX_RX_TERM_REG_NUM		0x2b
#define CORE_INT_TX_RX_TERM_VAL_DEFAULT_TC	2
#define CORE_INT_TX_RX_TERM_VAL_DEFAULT		1

#define CORE_INT_EI_THR_REG_NUM		0x2e
#define CORE_INT_EI_THR_SET(thr)	(((thr) << 8) | 0x10)
#define CORE_INT_EI_THR_ETH_DEFAULT	CORE_INT_EI_THR_SET(0x08)

#define CORE_INT_CRC_CHECK_REG_NUM		0x3c
#define CORE_INT_CRC_CHECK_VAL_VALID		0x00
#define CORE_INT_CRC_CHECK_VAL_FAIL		0xff
#define CORE_INT_CRC_CHECK_VAL_IN_PROGRESS	0x3c
#define CORE_INT_CRC_CHECK_TIMEOUT_US		20000

#define CORE_INT_PMD_CFG_REG_NUM		0x3d
#define CORE_INT_PMD_CFG_TRAINING_BHV_OVERLAY	0x2000
#define CORE_INT_PMD_CFG_TRAINING_BHV_FIXED	0x2001
#define CORE_INT_PMD_CFG_PRBS_SEQ_CLAUSE_92(l)	(0x3000 + (l))
#define CORE_INT_PMD_CFG_PRBS_SEQ_CLAUSE_92_NUM_LANES	4
#define CORE_INT_PMD_CFG_PRBS_SEQ_CLAUSE_72	0x3004
#define CORE_INT_PMD_CFG_PRBS_SEED(v)		(0x4000 | (v))

#define CORE_INT_BUILD_ID_GET_REG_NUM		0x3f

#define CORE_INT_GET_DFE_REG_NUM		0x126
#define CORE_INT_GET_DFE_VAL_DFE_STATUS		0x0b00
#define CORE_INT_GET_DFE_VAL_DFE_STATUS_IOC_COMPLETE		0x80
#define CORE_INT_GET_DFE_VAL_DFE_STATUS_PCAL_ADAPTIVE_EN	0xe2
#define CORE_INT_GET_DFE_VAL_DWELL_BIT_LO	0x0300
#define CORE_INT_GET_DFE_VAL_DWELL_BIT_HI	0x0400

#define CORE_INT_ESB_REG_RD(n)			(0x4000 | (n))
#define CORE_INT_ESB_REG_WR(n)			(0x8000 | (n))

/* Big registers */
#define BIG_REG_DFE_TUNING_DWELL_TIME_REG_NUM	7
#define BIG_REG_TBD1_REG_NUM			0x8026
#define BIG_REG_TBD1_O_LINK_LOOPBACK_EN		AL_BIT(2)

/* ESB registers */
#define ESB_REG_SIG_STAT_REG_NUM		0x25
#define ESB_REG_SIG_STAT_SIG_DETECTED_N		AL_BIT(2)

#define ESB_REG_SIG_LOSS_STAT_REG_NUM		0x26
#define ESB_REG_SIG_LOSS_STAT_SIG_LOST		AL_BIT(4)

#define ESB_REG_BIST_AUX_ERR_CTRL_0		0x0c
#define ESB_REG_BIST_AUX_ERR_CTRL_0_CFG_MASK	0x23
#define ESB_REG_BIST_AUX_ERR_CTRL_0_CFG_VAL	0x23
#define ESB_REG_BIST_AUX_ERR_CTRL_0_DIS_MASK	0x20
#define ESB_REG_BIST_AUX_ERR_CTRL_0_DIS_VAL	0x00

#define ESB_REG_BIST_AUX_ERR_CTRL_1		0x18
#define ESB_REG_BIST_AUX_ERR_CTRL_1_CFG_MASK	0x23
#define ESB_REG_BIST_AUX_ERR_CTRL_1_CFG_VAL	0x20
#define ESB_REG_BIST_AUX_ERR_CTRL_1_DIS_MASK	0x20
#define ESB_REG_BIST_AUX_ERR_CTRL_1_DIS_VAL	0x00

#define ESB_REG_BIST_AUX_ERR_CTRL_2		0x40
#define ESB_REG_BIST_AUX_ERR_CTRL_2_CFG_VAL	0x0580
#define ESB_REG_BIST_AUX_ERR_CTRL_2_DIS_VAL	0x0700

#define ESB_REG_BIST_AUX_ERR_CTRL_3		0x41
#define ESB_REG_BIST_AUX_ERR_CTRL_3_CFG_VAL_1	0x8540
#define ESB_REG_BIST_AUX_ERR_CTRL_3_CFG_VAL_2	0x8541
#define ESB_REG_BIST_AUX_ERR_CTRL_3_DIS_VAL	0x0700

#define ESB_REG_BIST_AUX_ERR_CNT_LO		0x46
#define ESB_REG_BIST_AUX_ERR_CNT_HI		0x47

#define ESB_REG_EI_STATUS_REG_NUM		0xc4
#define ESB_REG_EI_STATUS_RX_EN			AL_BIT(9)
#define ESB_REG_EI_STATUS_RX_THRESHOLD_MASK	AL_FIELD_MASK(4, 0)
#define ESB_REG_EI_STATUS_RX_THRESHOLD_SHIFT	0

#define CORE_INT_PLL_CAL_CF_MAX0_PCIE_REG_NUM	0x82f3
#define CORE_INT_PLL_CAL_CF_MAX0_PCIE_REG_VAL	0x7d
#define CORE_INT_PLL_CAL_CF_MIN1_PCIE_REG_NUM	0x82f4
#define CORE_INT_PLL_CAL_CF_MIN1_PCIE_REG_VAL	0x28
#define CORE_INT_PLL_CAL_CF_MAX2_PCIE_REG_NUM	0x82f5
#define CORE_INT_PLL_CAL_CF_MAX2_PCIE_REG_VAL	0x7d
#define CORE_INT_PLL_CAL_CF_MIN3_PCIE_REG_NUM	0x82f6
#define CORE_INT_PLL_CAL_CF_MIN3_PCIE_REG_VAL	0x28

#ifdef __cplusplus
}
#endif

#endif

/** @} */
