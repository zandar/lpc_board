/**
 * @file   can_ids.h
 * @author Michal Sojka <sojkam1@fel.cvut.cz>
 * @date   Wed Feb 25 14:28:26 2009
 * 
 * @brief  IDs of CAN bus messages
 *
 */
#ifndef _CAN_IDS_H
#define _CAN_IDS_H

#define to_boa(x) x
#define to_mot(x) x
#define to_per(x) x
#define to_las(x) x

/**
 * @addtogroup canmsg
 * @{
 * @nosubgrouping
 */

#define CAN_PWR_ALERT	to_boa(0x05)	/**< alert power status */

#define CAN_CORR_TRIG   to_boa(0x008)   /* ULoPoS: correlation started */
#define CAN_CORR_DIST   to_boa(0x009)   /* ULoPoS: measured distances */

#define CAN_ROBOT_CMD	to_boa(0x10)	/**< robot command (start, ..) */

#define CAN_ODO_RESET 	to_boa(0x14) /* ODO->BOA */
#define CAN_ODO_DATA 	to_boa(0x15) /* ODO->BOA */

#define CAN_MOTION_RESET           to_mot(0x20) /* BOA->MOT */
#define CAN_MOTION_CMD             to_mot(0x21) /* BOA->MOT */
#define CAN_MOTION_ODOMETRY_SIMPLE to_boa(0x22) /* MOT->BOA */
#define CAN_MOTION_STATUS          to_boa(0x23) /* MOT->BOA */



#define CAN_ROBOT_SWITCHES	to_boa(0x30)

// ids of can-peripherials
#define CAN_CHELAE to_per(0x32)          /**< BOA->PER @copydetails set_chelae() front view  1st B  left,  2nd B right */ 
#define CAN_ADC_1 to_boa(0x33)          /* PER->BOA */
#define CAN_ADC_2 to_boa(0x34)          /* PER->BOA */
#define CAN_IR    to_boa(0x35)          /* PER->BOA */
#define CAN_LED   to_per(0x36)          /* BOA->PER */
#define CAN_ADC_3 to_boa(0x37)		/* PER->BOA */

#define CAN_BELTS to_per(0x38)	/**< BOA->PER @copydetails set_belts()*/


#define CAN_PWR to_per(0x40)		/* BOA->PER */
					/* spodni 3 bity: zapnout. dalsi 3 b zapnout */

#define CAN_PWR_ADC1 to_boa(0x41)	/* PER->BOA */
#define CAN_PWR_ADC2 to_boa(0x42)	/* PER->BOA */
					/* napeti na jednotlivych vetvich, 4B hodnoty */

//#define CAN_BRUSHES_STATUS 	to_boa(0x44)	// FIXME: (F.J.) rename me, ...

#define CAN_PUCK		to_boa(0x43)


#define CAN_CMU	  		to_boa(0x46)		/* PER->BOA */
#define CAN_HOKUYO_PITCH  	to_per(0x47)		/* BOA->PER */

//#define CAN_ERROR		to_boa(0x48)	// FIXME: (F.J.) rename me, ...


#define CAN_VIDLE_STATUS 	to_boa(0x48)
#define CAN_VIDLE_CMD		to_per(0x49)

// #undef to_boa
// #undef to_mot
// #undef to_per
// #undef to_las

#define CAN_DB_LP		to_boa(0x100)	// debug messages


/** @{ */


#endif
