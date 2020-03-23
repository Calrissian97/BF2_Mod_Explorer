#pragma once
#include <string>
#include <vector>

class Side
{
public:
	Side();
	Side(std::string str);

	std::string SideFolder;
	std::vector<std::pair<std::string, std::string>> ODF;
	std::vector<std::pair<std::string, std::string>> MSH;
	std::vector<std::pair<std::string, std::string>> TGA;
	std::vector<std::pair<std::string, std::string>> REQ;
	std::vector<std::pair<std::string, std::string>> FX;
};

class World
{
public:
	World();

	std::vector<std::pair<std::string, std::string>> ODF;
	std::vector<std::pair<std::string, std::string>> MSH;
	std::vector<std::pair<std::string, std::string>> TGA;
	std::vector<std::pair<std::string, std::string>> REQ;
	std::vector<std::pair<std::string, std::string>> FX;
};

class Mod
{
public:
	Mod();
	Mod(std::string str);

	std::string ModFolder;
	std::vector<Side> Sides;
	std::vector<World> Worlds;
	std::pair<std::string, std::string> Addme;
	std::vector<std::pair<std::string, std::string>> Common_ODF;
	std::vector<std::pair<std::string, std::string>> Common_MSH;
	std::vector<std::pair<std::string, std::string>> Common_TGA;
	std::vector<std::pair<std::string, std::string>> Common_REQ;
	std::vector<std::pair<std::string, std::string>> Common_LUA;
	std::vector<std::pair<std::string, std::string>> Common_FX;
};
