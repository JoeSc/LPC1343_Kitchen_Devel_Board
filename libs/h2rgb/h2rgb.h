
#ifndef __H2RGB_H 
#define __H2RGB_H

#ifndef H2RGB_OUT_RANGE
	#define H2RGB_OUT_RANGE (4092)
#endif


void h2rgb(uint32_t H, uint32_t *R, uint32_t *G, uint32_t *B);
#endif
