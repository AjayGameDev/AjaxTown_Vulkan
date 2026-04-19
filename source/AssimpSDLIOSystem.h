#pragma once
#include <assimp/IOSystem.hpp>
#include <assimp/IOStream.hpp>

// Bridges one Assimp file open to SDL_IOFromFile
class AssimpSDLIOStream : public Assimp::IOStream
{
    SDL_IOStream* io = nullptr;
public:
    explicit AssimpSDLIOStream(const char* path)
    {
        io = SDL_IOFromFile(path, "rb");
    }
    ~AssimpSDLIOStream() { if (io) SDL_CloseIO(io); }

    bool IsOpen() const { return io != nullptr; }

    size_t Read(void* buf, size_t size, size_t count) override
    {
        return SDL_ReadIO(io, buf, size * count) / size;
    }
    aiReturn Seek(size_t offset, aiOrigin origin) override
    {
        SDL_IOWhence whence = (origin == aiOrigin_SET) ? SDL_IO_SEEK_SET
                            : (origin == aiOrigin_CUR) ? SDL_IO_SEEK_CUR
                                                       : SDL_IO_SEEK_END;
        return SDL_SeekIO(io, offset, whence) >= 0 ? aiReturn_SUCCESS : aiReturn_FAILURE;
    }
    size_t Tell() const override { return (size_t)SDL_TellIO(io); }
    size_t FileSize() const override
    {
        size_t cur = Tell();
        SDL_SeekIO(io, 0, SDL_IO_SEEK_END);
        size_t size = Tell();
        SDL_SeekIO(io, cur, SDL_IO_SEEK_SET);
        return size;
    }
    void Flush() override {}
    size_t Write(const void*, size_t, size_t) override { return 0; } // read-only
};

// Plugs into Assimp's Importer
class AssimpSDLIOSystem : public Assimp::IOSystem
{
public:
    bool Exists(const char* path) const override
    {
        SDL_IOStream* io = SDL_IOFromFile(path, "rb");
        if (io) { SDL_CloseIO(io); return true; }
        return false;
    }
    char getOsSeparator() const override { return '/'; }

    Assimp::IOStream* Open(const char* path, const char* mode = "rb") override
    {
        auto* stream = new AssimpSDLIOStream(path);
        if (!stream->IsOpen()) { delete stream; return nullptr; }
        return stream;
    }
    void Close(Assimp::IOStream* stream) override { delete stream; }
};