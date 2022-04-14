#include "lab5/lab5.h"
#include <stdio.h>

void main(void)
{
	init();
	init_sensor();

	while (1)
	{
		if (get_contact_error_flag())
		{
			set_contact_error_flag(0);
			draw_error(1);
		}
		else if (get_but2_flag())
		{
			sensor_trigger();
			set_but2_flag(0);
		}
		else if (get_echo_flag())
		{
			float dist = (SND_SPEED * get_rtt_time() * 100) / (2 * FREQUENCY);
			if (dist > 400 || dist < 2)
			{
				draw_error(0);
				// deu ruim
			}
			else
			{
				draw_distance(dist);
				// desenha dist e grafico
			}
		}
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
	}
}