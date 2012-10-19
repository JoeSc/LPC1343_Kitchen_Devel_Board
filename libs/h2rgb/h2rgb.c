#include "LPC13xx.h"			/* LPC13xx Peripheral Registers */
#include "h2rgb.h"

void h2rgb(uint32_t H, uint32_t *R, uint32_t *G, uint32_t *B)
{
	uint32_t f, i,n;
	f = H % H2RGB_OUT_RANGE;
	i = H -f;

	if (( i/H2RGB_OUT_RANGE) % 2 == 0)
		f = H2RGB_OUT_RANGE - f;
	n = ( 6* (H2RGB_OUT_RANGE - f) * H2RGB_OUT_RANGE) / (H2RGB_OUT_RANGE * 6);
	switch(i)
	{
		case 0:
		case (H2RGB_OUT_RANGE * 6):
			*R = H2RGB_OUT_RANGE;
			*G = n;
			*B = 0;
			break;
		case (H2RGB_OUT_RANGE):
			*R = n;
			*G = H2RGB_OUT_RANGE;
			*B = 0;
			break;
		case (H2RGB_OUT_RANGE * 2):
			*R = 0;
			*G = H2RGB_OUT_RANGE;
			*B = n;
			break;
		case (H2RGB_OUT_RANGE * 3):
			*R = 0;
			*G = n;
			*B = H2RGB_OUT_RANGE;
			break;
		case (H2RGB_OUT_RANGE * 4):
			*R = n;
			*G = 0;
			*B = H2RGB_OUT_RANGE;
			break;
		case (H2RGB_OUT_RANGE * 5):
			*R = H2RGB_OUT_RANGE;
			*G = 0;
			*B = n;
	}
}
