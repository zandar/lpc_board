/**
 * @file   	map_definitions.h
 * @brief	Map useful defines
 * @author	Jose Maria Martin Laguna <jmmartin@etud.insa-toulouse.fr>
 *
*/
/** @ingroup pp */
/**@{*/
#define MAP_WIDTH	30      /**< @brief Field width*/
#define	MAP_HEIGHT	21      /**< @brief Field height*/

#define WALL_COST	1000	/**< @brief Cost fo jumping a wall. */ 

#define MAP_WALL		0xFF	/**< @brief Wall cell. Black in robomon.*/ 
#define MAP_PATH		0xFE	/**< @brief Path cell. Darkred in robomon.*/ 
#define MAP_START		0xFD	/**< @brief Start cell. Red in robomon.*/ 
#define MAP_GOAL		0xFC	/**< @brief Goal cell. Green in robomon.*/
#define MAP_NEW_OBSTACLE	0xFB	/**< @brief Fouded obstacle cell. Blue in robomon.*/
#define MAP_FREE		0x00	/**< @brief Free cell. The robot can move on. White in robomon.*/ 

#define SHM_MAP_KEY	555		/**< @brief Key use to share the memory SHM*/ 

// some macros to acces to map
#define GETMAPPOS(i,j) 		(*(map+i+j*MAP_WIDTH))

// Some macros useful to convert from Graph coordonates to robot field
#define GRAPHX2FIELD(x)		((x+0.5)/10.0)	/**< @brief Convert from cell coordonates X to real coordonates. Use better cell2real_X(). */	
#define GRAPHY2FIELD(y)		((abs(y-MAP_HEIGHT)-0.5)/10.0) /**< @brief Convert from cell coordonates Y to real coordonates. Use better cell2real_Y(). */

typedef unsigned char map_t;
/**@}*/