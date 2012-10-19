


def h2rgb(H,p):
	i = H - (H % 40)
	f = H -i;
	if (( i/40) %2 == 0):
		f = 40 -f
	n = (6*(40-f)*255)/240
	if (i == 240) or (i == 0):
		red = 255
		green = n
		blue = 0
	elif i == 40:
		red = n
		green = 255
		blue = 0
	elif i == 80:
		red = 0
		green = 255
		blue = n
	elif i == 120:
		red = 0
		green = n
		blue = 255
	elif i == 160:
		red = n
		green = 0
		blue = 255
	elif i == 200:
		red = 255
		green = 0
		blue = n
	else:
		print i

	print "%3d, %3d, %3d, %3d"%(p,red,green,blue)


for i in range(0,480):
	h2rgb(i%240,i)

