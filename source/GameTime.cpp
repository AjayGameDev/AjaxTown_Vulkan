#include "GameTime.h"

GameTime::GameTime()
{
	currentTicks    =   0;
	frequency       =   0;
	accumulator     =   0;
	deltaTime       =   0;
	alpha           =   0;
	fixedDeltaTime  =   1.0 / 60.0;                    // for 60FPS in seconds 1/60 will give zero as int division
	maxDeltaTime    =   0.250;                        // 250 ms in seconds
	previousTicks   =   SDL_GetPerformanceCounter(); // To avoid getting zero on the first frame
}

void GameTime::Update()
{
	// Calculating deltaTime

	currentTicks  = SDL_GetPerformanceCounter();     // Total ticks happend since start of the program [uint64_t]
	frequency     = SDL_GetPerformanceFrequency();  // Ticks per seconds example: 1 second = 1000 ticks => 1 tick = 1/1000 seconds = 1/F
                                                   // ticks happend since last frame  = deltaTicks = currentTicks - previousTicks
                                                  // deltaTime = deltaTicks * time it takes to complete 1 tick = deltaTicks * 1/F
                                                 // Multiply by 1000 to get deltaTime in milliSeconds
                                                // store it as double instead of float[read article in .h]
	if (frequency <= 0)
		return;
	deltaTime     = static_cast<double>(currentTicks - previousTicks) / frequency;
	previousTicks = currentTicks;

	if (deltaTime > maxDeltaTime)
		deltaTime = maxDeltaTime;

	accumulator += deltaTime;

	if (accumulator >= fixedDeltaTime)
	{
		//FixedUpdate(fixedDeltaTime);
		accumulator -= fixedDeltaTime;
	}

	alpha = accumulator / fixedDeltaTime;
	
	//Update(deltaTime);
	//Render(alpha);
}
