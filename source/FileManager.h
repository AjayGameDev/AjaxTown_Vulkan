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


			static std::vector<uint8_t>  LoadBytes_8bit(const std::string& filePath);
			static std::vector<uint32_t> LoadBytes_32bit(const std::string& filePath);
			static string GetLocation(Location location);


    private:
			// only declare here, will be defined in cpp file 
			static unordered_map<Location, string> locations; 
			static void Initialize();
			

			
			FileManager() = delete;  // to avoid instantiation

};

