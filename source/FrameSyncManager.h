#pragma once
#include <vector>
#include "Fence.h"
#include "Semaphore.h"


struct FrameSync
{
    Fence     frameFence;
    Semaphore imageAvailableSemaphore;
    Semaphore renderFinishedSemaphore;
};


class FrameSyncManager
{
    private:
            uint32_t currentFrame = 0;
            uint32_t maxFramesInFlight;
            std::vector<FrameSync> framesSync;
    public:
            explicit FrameSyncManager(uint32_t maxFramesInFlight) : maxFramesInFlight(maxFramesInFlight),framesSync(maxFramesInFlight) {}
            FrameSync& GetCurrentFrameSync() { return framesSync[currentFrame]; }
            void NextFrame() { currentFrame = (currentFrame + 1) % maxFramesInFlight; }
};
