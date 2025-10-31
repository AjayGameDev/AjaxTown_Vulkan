#pragma once




class GameTime
{
    public:
		    double deltaTime; // delta time in seconds
		    GameTime();
		    void Update();
    private:
		    uint64_t currentTicks, previousTicks,frequency;
		    double   fixedDeltaTime, accumulator, maxDeltaTime;
                    double   alpha; // for rendering interpolated position btw physics update for smoother movement
                                   //Don't use float for time, use double or uint64 from this article https://randomascii.wordpress.com/2012/02/13/dont-store-that-in-a-float/

};



