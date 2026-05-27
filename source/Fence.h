#pragma once

class Fence 
{
    public:
            explicit Fence(Context& context,bool isSignaled = true);
            ~Fence();

            void Create(bool isSignaled);

            // Delete copy constructors
            Fence(const Fence&) = delete;
            Fence& operator=(const Fence&) = delete;

            // Move constructors
            Fence(Fence&& other) noexcept:context(other.context),handle(other.handle)
            {
                other.handle = nullptr;
            }

            Fence& operator=(Fence&& other) noexcept
            {
                if (this != &other)
                {
                    // Destroy any existing fence
                    Destroy();

                    handle = other.handle;
                    // context is a reference, cannot be reassigned but is assumed same
                    other.handle = VK_NULL_HANDLE;
                }

                return *this;
            }

            VkFence& GetHandle()
            {
                return handle;
            }

            inline void Wait(uint64_t timeout = UINT64_MAX)
            {
                vkWaitForFences(context.device,1,&handle,VK_TRUE,timeout);
            }

            inline void Reset()
            {
                vkResetFences(context.device,1,&handle);
            }

            inline void WaitAndReset(uint64_t timeout = UINT64_MAX)
            {
                vkWaitForFences(context.device,1,&handle,VK_TRUE,timeout);
                vkResetFences(context.device,1,&handle);
            }

            inline bool IsSignaled()
            {
                VkResult result = vkGetFenceStatus(context.device,handle);

                if (result == VK_SUCCESS)
                    return true;

                if (result == VK_NOT_READY)
                    return false;

                throw std::runtime_error("\nError occured when trying to check fence status" + std::to_string(result) );
            }

            void Destroy()
            {
                if (handle  !=  VK_NULL_HANDLE)
                {
                    vkDestroyFence(context.device,handle,nullptr);
                    handle = VK_NULL_HANDLE;
                }
            }

    private:
            Context& context;
            VkFence handle = VK_NULL_HANDLE;
};
