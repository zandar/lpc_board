/* VHN2SP30 H-bridge control */

#ifndef  __VHN_H
#define  __VHN_H

  #include "def.h"

  #define VHN_EN_A		(1<<16) /* P1.16 = GPIO1 = GPIO port 2 */
  #define VHN_EN_B		(1<<17) /* P1.17 = GPIO2 = GPIO port 1 */
  #define VHN_IN_A		(1<<18) /* P1.18 = GPIO3 = GPIO port 4 */
  #define VHN_IN_B		(1<<19) /* P1.19 = GPIO4 = GPIO port 3 */


    void vhn_init(void);
    void vhn_reset(void);
    void vhn_speed(uint8_t speed, uint8_t dir);

	
	


#endif /*__VHN_H*/
