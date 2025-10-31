#pragma once


class Semaphore 
{
    private:
            VkSemaphore handle = VK_NULL_HANDLE;
            Context& context;

        public:
            explicit Semaphore(Context& context);
            ~Semaphore();

            // Delete copy constructors
            Semaphore(const Semaphore&) = delete;
            Semaphore& operator=(const Semaphore&) = delete;

            // movable
            Semaphore(Semaphore&& other) noexcept : context(other.context), handle(other.handle)
            {
                other.handle = VK_NULL_HANDLE;
            }
            Semaphore& operator=(Semaphore&& other) noexcept
            {
                if (this != &other)
                {
                    Destroy();
                    handle = other.handle;
                    other.handle = VK_NULL_HANDLE;
                }
                return *this;
            }

            void Create();
            void Destroy();
            VkSemaphore& GetHandle() { return handle; } // functions defined inside the class are implicitly inline

};
