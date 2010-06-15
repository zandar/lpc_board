/*
 * can_msg_masks.h			09/04/08
 *
 * Bitmasks for can messages.
 *
 * Copyright: (c) 2008 CTU Dragons
 *            CTU FEE - Department of Control Engineering
 * License: GNU GPL v.2
 */

#ifndef _CAN_MSG_MASKS_H
#define _CAN_MSG_MASKS_H

#include <sys/types.h>

#define CAN_MSG_MASK(x)		((u_int8_t)1 << (x))

#define CAN_PWR_CTRL_33_ON		CAN_MSG_MASK(0)
#define CAN_PWR_CTRL_50_ON		CAN_MSG_MASK(1)
#define CAN_PWR_CTRL_80_ON		CAN_MSG_MASK(2)

#define CAN_PWR_CTRL_33_OFF		CAN_MSG_MASK(3)
#define CAN_PWR_CTRL_50_OFF		CAN_MSG_MASK(4)
#define CAN_PWR_CTRL_80_OFF		CAN_MSG_MASK(5)

#endif /* _CAN_MSG_MASKS_H */

