#define RAMP_DOWN_TIME_MS (1000)
#define RGB_POWER_OFF_TIME_MS (30*1000)
#define W_POWER_OFF_TIME_MS (10*1000)

uint64_t last_motion=0;
//Attach interrupt to motion pin.  
motion_interrupt
{
	last_motion = msTicks;
}
void transfer_to_sleep()
{
	// slowdown ADC??? Worth it?
	//disable PLL??? Worth it?
	while (msTicks > last_motion)
		WFI;
}

	
	

int white_on = 0;
int rgb_on = 0;
int ww_sw_asserted = 0;
int rgb_sw_asserted = 0;
int off_sw_asserted = 0;
uint32_t ww_scale = 0;
uint32_t rgb_scale = 0;
main()
{
	INIT Everything
	Set PWMs to off
	setup_motion_pin_interrupt()
	do this every ms
	{



		if ((msTicks - last_motion ) > 
			max(W_POWER_OFF_TIME_MS + RAMP_DOWN_TIME_MS,
				RGB_POWER_OFF_TIME_MS + RAMP_DOWN_TIME_MS ))
		{
			shut_off_supply();
			go_to_sleep();
			turn_on_supply();
			/* Wait for the power to actually come on before starting the ramp up */
			while(! getSwitch(POWER_IND));
		}

		// WW_SWITCH_HANDLER
		// debounce for 20ms, since we check for the absolute val if the user
		// holds the switch forever it will only trigger the service once
		if(getSwitch(WW_SWITCH) == 0)
		{
			if(ww_sw_asserted == 20)
			{
				white_on = !white_on;
				white_scale = 0;
			}
			ww_sw_asserted += 1;
			// Might as well update last_motion...
			last_motion = msTicks;
		}
		else
			ww_sw_asserted = 0;

		// OFF_SWITCH_HANDLER
		// debounce for 20ms, since we check for the absolute val if the user
		// holds the switch forever it will only trigger the service once
		if(getSwitch(OFF_SWITCH) == 0)
		{
			if(off_sw_asserted == 20)
			{
				white_on = 0;
				white_scale = 0;
				rgb_on = 0;
				rgb_scale = 0;
			}
			off_sw_asserted += 1;
			// Might as well update last_motion...
			last_motion = msTicks;
		}
		else
			off_sw_asserted = 0;
		
		// RGB_SWITCH_HANDLER
		// debounce for 20ms, since we check for the absolute val if the user
		// holds the switch forever it will only trigger the service once
		if(getSwitch(RGB_SWITCH) == 0)
		{
			if(rgb_sw_asserted == 20)
			{
				rgb_on = !rgb_on;
				rgb_scale = 0;
			}
			if(rgb_sw_asserted == 2000)
			{
				rgb_on = 2;
			}
			rgb_sw_asserted += 1;
			// Might as well update last_motion...
			last_motion = msTicks;
		}
		else
			rgb_sw_asserted = 0;




		/* This handles the WW LEDs */
		if (((msTicks - last_motion ) > W_POWER_OFF_TIME_MS) && ww_on)
		{
			white_scale = RAMP_DOWN_TIME_MS - ((msTicks - last_motion) - W_POWER_OFF_TIME_MS);
			if ((msTicks - last_motion) > (W_POWER_OFF_TIME_MS + RAMP_DOWN_TIME_MS))
				white_scale = 0;
		}
		if ( white_scale < RAMP_DOWN_TIME_MS && white_on && ((msTicks - last_motion ) < W_POWER_OFF_TIME_MS))
		{
			white_scale = msTicks - last_motion;
			/* Check just in case we get held up and skip the 1000th ms after motion */
			if ( white_scale > RAMP_DOWN_TIME_MS)
				white_scale = RAMP_DOWN_TIME_MS;
		}
		if (white_scale != RAMP_DOWN_TIME_MS)
			setWW( (white_scale * getADC(WW_POT)) / RAMP_DOWN_TIME_MS );
		else
			setWW(getADC(WW_POT));
		
			

		/* This handles the RGB LEDs */
		if (((msTicks - last_motion ) > RGB_POWER_OFF_TIME_MS) && rgb_on)
		{
			rgb_scale = RAMP_DOWN_TIME_MS - ((msTicks - last_motion) - RGB_POWER_OFF_TIME_MS);
			if ((msTicks - last_motion) > (RGB_POWER_OFF_TIME_MS + RAMP_DOWN_TIME_MS))
				rgb_scale = 0;
		}
		if ( rgb_scale < RAMP_DOWN_TIME_MS && rgb_on && ((msTicks - last_motion ) < RGB_POWER_OFF_TIME_MS))
		{
			rgb_scale = msTicks - last_motion;
			/* Check just in case we get held up and skip the 1000th ms after motion */
			if ( rgb_scale > RAMP_DOWN_TIME_MS)
				rgb_scale = RAMP_DOWN_TIME_MS;
		}

		if (rgb_on == 1)
		{
			if (rgb_scale != RAMP_DOWN_TIME_MS)
				setRGB((rgb_scale * getADC(RED_POT)) / RAMP_DOWN_TIME_MS,
						(rgb_scale * getADC(GREEN_POT)) / RAMP_DOWN_TIME_MS,
						(rgb_scale * getADC(BLUE_POT)) / RAMP_DOWN_TIME_MS );
			else
				setRGB(getADC(RED_POT), getADC(GREEN_POT), getADC(BLUE_POT));
		}
		else if (rgb_on == 2)
		{
			uint32_t R,G,B;
			h2rgb(TIME, &R, &G, &B);
			if (rgb_scale != RAMP_DOWN_TIME_MS)
				setRGB((rgb_scale * R) / RAMP_DOWN_TIME_MS,
						(rgb_scale * G) / RAMP_DOWN_TIME_MS,
						(rgb_scale * B) / RAMP_DOWN_TIME_MS );
			else
				setRGB(R, G, B);
		}

	}
}


