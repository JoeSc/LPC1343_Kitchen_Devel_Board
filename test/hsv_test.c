
#define OUT_RANGE (1023)

inline  int h2rgb(unsigned long H, unsigned short *R, unsigned short *G, unsigned short *B)
{
	volatile int f, i,n;
	f = H % OUT_RANGE;
	i = H -f;

	if (( i/OUT_RANGE) % 2 == 0)
		f = OUT_RANGE - f;
	n = ( 6* (OUT_RANGE - f) * OUT_RANGE) / (OUT_RANGE * 6);
	switch(i)
	{
		case 0:
		case (OUT_RANGE * 6):
			*R = OUT_RANGE;
			*G = n;
			*B = 0;
			break;
		case (OUT_RANGE):
			*R = n;
			*G = OUT_RANGE;
			*B = 0;
			break;
		case (OUT_RANGE * 2):
			*R = 0;
			*G = OUT_RANGE;
			*B = n;
			break;
		case (OUT_RANGE * 3):
			*R = 0;
			*G = n;
			*B = OUT_RANGE;
			break;
		case (OUT_RANGE * 4):
			*R = n;
			*G = 0;
			*B = OUT_RANGE;
			break;
		case (OUT_RANGE * 5):
			*R = OUT_RANGE;
			*G = 0;
			*B = n;
	}
}
