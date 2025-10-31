#pragma once


using std::unordered_map;
using std::string;


class FileManager
{
	public:
			enum Location
			{
				Model,
				Shader,
				Texture,
				Skybox,
				Fonts,
				Config
			};



			static string GetLocation(Location location);
			

    private:
			// only declare here, will be defined in cpp file 
			static unordered_map<Location, string> locations; 
			static void Initialize();
			

			
			FileManager() = delete;  // to avoid instantiation

};

