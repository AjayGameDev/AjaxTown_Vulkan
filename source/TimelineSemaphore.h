#pragma once


class TimelineSemaphore 
{


    VkSemaphore handle = VK_NULL_HANDLE;
    Context& context;

public:
    explicit TimelineSemaphore(Context& context);
    ~TimelineSemaphore();

    void Create();
    void Destroy();

    VkSemaphore& GetHandle() { return handle; }


    // Delete copy constructors
    TimelineSemaphore(const TimelineSemaphore&) = delete;
    TimelineSemaphore& operator=(const TimelineSemaphore&) = delete;

    // movable
    TimelineSemaphore(TimelineSemaphore&& other) noexcept : handle(other.handle), context(other.context)
    {
        other.handle = VK_NULL_HANDLE;
    }
    TimelineSemaphore& operator=(TimelineSemaphore&& other) noexcept
    {
        if (this != &other)
        {
            Destroy();
            handle = other.handle;
            other.handle = VK_NULL_HANDLE;
        }
        return *this;
    }

};
