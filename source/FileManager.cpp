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
	locations[Model] = "C:\\Ajay\\Ajax Town\\assets\\Model\\";

	locations[Shader] = "C:\\Ajay\\Ajax Town\\assets\\Shaders\\";

	locations[Texture] = "C:\\Ajay\\Ajax Town\\assets\\Textures\\";

	locations[Skybox] = "C:\\Ajay\\Ajax Town\\assets\\Textures\\HDRI\\";

	locations[Fonts] = "C:\\Ajay\\Ajax Town\\assets\\Fonts\\";

	locations[Config] = "C:\\Ajay\\Ajax Town\\assets\\Configuration\\";


}
