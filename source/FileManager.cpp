#include "FileManager.h"

// Create the definition outside of the class in cpp file
unordered_map<FileManager::Location, string> FileManager::locations;

string FileManager::GetLocation(Location location)
{
	static bool initialized = false;

	if (!initialized)
	{
		Initialize();
		initialized = true;
	}

	return locations[location];
}


void FileManager::Initialize()
{

#ifdef __ANDROID__
	string base = "";
#else
	string base = "C:/Users/dubey/CLionProjects/AjaxTown/assets/";
#endif

	locations[Model]   = base + "model/";

	locations[Shader]  = base + "shaders/compiled/";

	locations[Texture] = base + "textures/";

	locations[Skybox]  = base + "textures/HDRI/";

	locations[Fonts]   = base + "fonts/";

	locations[Config]  = base + "configuration/";


}
