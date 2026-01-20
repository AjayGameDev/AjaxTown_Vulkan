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
	locations[Model] = "C:\\Users\\dubey\\CLionProjects\\AjaxTown\\assets\\model\\";

	locations[Shader] = "C:\\Users\\dubey\\CLionProjects\\AjaxTown\\assets\\shaders\\compiled\\";

	locations[Texture] = "C:\\Users\\dubey\\CLionProjects\\AjaxTown\\assets\\textures\\";

	locations[Skybox] = "C:\\Users\\dubey\\CLionProjects\\AjaxTown\\assets\\textures\\HDRI\\";

	locations[Fonts] = "C:\\Users\\dubey\\CLionProjects\\AjaxTown\\assets\\fonts\\";

	locations[Config] = "C:\\Users\\dubey\\CLionProjects\\AjaxTown\\assets\\configuration\\";


}
