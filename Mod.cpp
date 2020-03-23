#include "Mod.h"

Mod::Mod()
{
	this->ModFolder = "";
	this->Addme = { "", "" };
	this->Sides.reserve(2);
	this->Worlds.reserve(2);
	this->Common_ODF.reserve(85);
	this->Common_MSH.reserve(155);
	this->Common_TGA.reserve(480);
	this->Common_REQ.reserve(230);
	this->Common_LUA.reserve(90);
	this->Common_FX.reserve(175);
}

Mod::Mod(std::string str)
{
	this->ModFolder = str;
	this->Addme = { "", "" };
	this->Sides.reserve(2);
	this->Worlds.reserve(2);
	this->Common_ODF.reserve(85);
	this->Common_MSH.reserve(155);
	this->Common_TGA.reserve(480);
	this->Common_REQ.reserve(230);
	this->Common_LUA.reserve(90);
	this->Common_FX.reserve(175);
}

Side::Side()
{
	this->SideFolder = "";
	this->ODF.reserve(256);
	this->MSH.reserve(256);
	this->TGA.reserve(256);
	this->REQ.reserve(128);
	this->FX.reserve(16);
}

Side::Side(std::string str)
{
	this->SideFolder = str;
	this->ODF.reserve(256);
	this->MSH.reserve(256);
	this->TGA.reserve(256);
	this->REQ.reserve(128);
	this->FX.reserve(16);
}

World::World()
{
	this->ODF.reserve(128);
	this->MSH.reserve(128);
	this->TGA.reserve(128);
	this->REQ.reserve(128);
	this->FX.reserve(32);
}