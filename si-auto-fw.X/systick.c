/*
 * systick.c
 *
 * Created: 18-05-2023 15:44:05
 *  Author: Mathias
 */ 

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>


volatile uint32_t systickTicks = 0;

void systick_isr(void)
{
	systickTicks++;
}

uint32_t SysTick_GetTicks(void)
{
	uint32_t curTicks;
	// Need atomic access to systickTicks
	di();	// disable IRQs
	curTicks = systickTicks;
	ei();	// enable IRQs
	return curTicks;
}

bool SysTick_CheckElapsed(uint32_t initialTicks, uint32_t delayTicks)
{
	if (SysTick_GetTicks() - initialTicks >= delayTicks) return true; // Delay elapsed = true
	return false; // Otherwise delay elapsed = false
}

// blocking delay
void SysTick_Delay (uint32_t delayTicks)
{
	uint32_t curTicks;
	curTicks = SysTick_GetTicks();

	// Make sure delay is at least 1 tick in case of division, etc.
	if (delayTicks == 0) delayTicks = 1;

	if (curTicks > 0xFFFFFFFF - delayTicks)
	{
		// Rollover will occur during delay
		while (SysTick_GetTicks() >= curTicks)
		{
			while (SysTick_GetTicks() < (delayTicks - (0xFFFFFFFF - curTicks)));
		}
	}
	else
	{
		while ((SysTick_GetTicks() - curTicks) < delayTicks);
	}
}