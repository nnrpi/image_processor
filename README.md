This program can apply some filters to your photo
Your photo should be in BMP 24-bit format
Argument format:
	1) path to binary file
	2) path to input file (your photo)
	3) path to output file
	4) some of filers you want to apply in format <-filter_name> <list of parameters if they need>
Filters:
	1) crop: crop your photo from the left top angle, parameters are new height and width in pixels (integers)
	2) gs: convert to gray shades, no parameters need
	3) neg: convert to negative, no parameters need
	4) sharp: increase sharpness, no parameters need
	5) edge: highlight edges of objects in the photo, parameter is an integer number from 0 to 255
	6) blur: blur your photo, parameter is an integer number - the more this number the more blur applies
	7) acos: somehow convert the colours of your photo
