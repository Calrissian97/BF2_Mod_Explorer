#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>
#include "Mod.h"
#include <utility>
#include <thread>
#include <filesystem>
#include <fstream>
#include <streambuf>
#pragma comment(lib,"shell32.lib")
#pragma once

// Declare Globals
namespace fs = std::filesystem;
std::vector<Mod> ModFolders;

// Declare Global Constants
const fs::path HOMEPATH(fs::current_path());
const unsigned int CORE_COUNT = 2; //std::thread::hardware_concurrency();

// Helper function
bool pattern_match(std::string str, std::string pattern) {
	enum State {
		Exact,      	// exact match
		Any,        	// ?
		AnyRepeat    	// *
	};

	const char* s = str.c_str();
	const char* p = pattern.c_str();
	const char* q = 0;
	int state = 0;

	bool match = true;
	while (match && *p) {
		if (*p == '*') {
			state = AnyRepeat;
			q = p + 1;
		}
		else if (*p == '?') state = Any;
		else state = Exact;

		if (*s == 0) break;

		switch (state) {
		case Exact:
			match = *s == *p;
			s++;
			p++;
			break;

		case Any:
			match = true;
			s++;
			p++;
			break;

		case AnyRepeat:
			match = true;
			s++;

			if (*s == *q) p++;
			break;
		}
	}

	if (state == AnyRepeat) return (*s == *q);
	else if (state == Any) return (*s == *p);
	else return match && (*s == *p);
}

// Adds Datafolder directories by creating Mod objects
void addDataFolders() noexcept
{
	// Add all datafolders to the vector
	for (auto& d : fs::directory_iterator(fs::current_path()))
		try 
		{
			if (d.is_directory() && pattern_match(d.path().filename().string(), "data_*"))
				ModFolders.emplace_back(Mod(d.path().filename().string()));
		}
		catch(...)
		{
			;
		}
}

// Adds Side directories by adding Side objects to a Mod
void addSideFolders() noexcept
{
		// Add all side folders to the vector of vectors
		for (unsigned int f = 0; f < ModFolders.size(); ++f)
		{
			try
			{
				if (fs::exists(fs::current_path() / ModFolders[f].ModFolder / "Sides"))
				{
					for (auto& e : fs::directory_iterator(fs::current_path() / ModFolders[f].ModFolder / "Sides"))
						if (e.is_directory())
							ModFolders[f].Sides.emplace_back(Side(e.path().filename().string()));
				}
			}
			catch (...) {};
		}
}

void addWorldFiles(Mod &CurrentMod) noexcept
{
	// Attempt to find all files in the Worlds Dir
	// Always use try{}Catch(...){} in case a folder is missing 
	try
	{
		// Place each file in it's catagorized parent node
		if (fs::exists(fs::current_path() / CurrentMod.ModFolder / "Worlds"))
		{
			World NewWorld;

			for (auto& r : fs::recursive_directory_iterator(fs::current_path() / CurrentMod.ModFolder / "Worlds"))
			{
				if (!fs::is_directory(r.path()) && r.path().has_extension())
				{
					if (r.path().extension().string() == ".odf")
					{
						NewWorld.ODF.emplace_back(std::make_pair(r.path().filename().string().substr(0, r.path().filename().string().length() - 4), r.path().string()));
					}
					else if (r.path().extension().string() == ".msh")
					{
						NewWorld.MSH.emplace_back(std::make_pair(r.path().filename().string().substr(0, r.path().filename().string().length() - 4), r.path().string()));
					}
					else if (r.path().extension().string() == ".tga")
					{
						NewWorld.TGA.emplace_back(std::make_pair(r.path().filename().string().substr(0, r.path().filename().string().length() - 4), r.path().string()));
					}
					else if (r.path().extension().string() == ".fx")
					{
						NewWorld.FX.emplace_back(std::make_pair(r.path().filename().string().substr(0, r.path().filename().string().length() - 3), r.path().string()));
					}
					else if (r.path().extension().string() == ".req")
					{
						NewWorld.REQ.emplace_back(std::make_pair(r.path().filename().string().substr(0, r.path().filename().string().length() - 4), r.path().string()));
					}
				}
			}

			CurrentMod.Worlds.emplace_back(NewWorld);
		}
		/*else
		{
			World NewWorld;

			// This is so dumb but idk how else to keep the vectors aligned
			std::pair<std::string, std::string> World_ODF_Temp;
			std::pair<std::string, std::string> World_MSH_Temp;
			std::pair<std::string, std::string> World_TGA_Temp;
			std::pair<std::string, std::string> World_FX_Temp;
			std::pair<std::string, std::string> World_REQ_Temp;

			NewWorld.ODF.emplace_back(World_ODF_Temp);
			NewWorld.MSH.emplace_back(World_MSH_Temp);
			NewWorld.TGA.emplace_back(World_TGA_Temp);
			NewWorld.FX.emplace_back(World_FX_Temp);
			NewWorld.REQ.emplace_back(World_REQ_Temp);

			CurrentMod.Worlds.emplace_back(NewWorld);
		}*/
	}
	catch (...)
	{
		;
	}
}

void addCommonFiles(Mod &CurrentMod) noexcept
{
	// Always use try{}Catch(...){} in case a folder is missing 
	try
	{
		// Place each file in it's catagorized parent node
		if (fs::exists(fs::current_path() / CurrentMod.ModFolder / "Common"))
		{
			for (auto& r : fs::recursive_directory_iterator(fs::current_path() / CurrentMod.ModFolder / "Common"))
			{
				if (!fs::is_directory(r.path()) && r.path().has_extension())
				{

					if (r.path().extension().string() == ".odf")
					{
						CurrentMod.Common_ODF.emplace_back(std::make_pair(r.path().filename().string().substr(0, r.path().filename().string().length() - 4), r.path().string()));
					}
					else if (r.path().extension().string() == ".msh")
					{
						CurrentMod.Common_MSH.emplace_back(std::make_pair(r.path().filename().string().substr(0, r.path().filename().string().length() - 4), r.path().string()));
					}
					else if (r.path().extension().string() == ".tga")
					{
						CurrentMod.Common_TGA.emplace_back(std::make_pair(r.path().filename().string().substr(0, r.path().filename().string().length() - 4), r.path().string()));
					}
					else if (r.path().extension().string() == ".lua")
					{
						CurrentMod.Common_LUA.emplace_back(std::make_pair(r.path().filename().string().substr(0, r.path().filename().string().length() - 4), r.path().string()));
					}
					else if (r.path().extension().string() == ".fx")
					{
						CurrentMod.Common_FX.emplace_back(std::make_pair(r.path().filename().string().substr(0, r.path().filename().string().length() - 3), r.path().string()));
					}
					else if (r.path().extension().string() == ".req")
					{
						CurrentMod.Common_REQ.emplace_back(std::make_pair(r.path().filename().string().substr(0, r.path().filename().string().length() - 4), r.path().string()));
					}
				}
			}
		}
		/*else
		{
			// This is so dumb but idk how else to keep the vectors aligned
			std::pair<std::string, std::string> Common_ODF_Temp;
			std::pair<std::string, std::string> Common_MSH_Temp;
			std::pair<std::string, std::string> Common_TGA_Temp;
			std::pair<std::string, std::string> Common_LUA_Temp;
			std::pair<std::string, std::string> Common_FX_Temp;
			std::pair<std::string, std::string> Common_REQ_Temp;

			CurrentMod.Common_ODF.emplace_back(Common_ODF_Temp);
			CurrentMod.Common_MSH.emplace_back(Common_MSH_Temp);
			CurrentMod.Common_TGA.emplace_back(Common_TGA_Temp);
			CurrentMod.Common_LUA.emplace_back(Common_LUA_Temp);
			CurrentMod.Common_FX.emplace_back(Common_FX_Temp);
			CurrentMod.Common_REQ.emplace_back(Common_REQ_Temp);
		}*/
	}
	catch (...)
	{
		;
	}
}

void addSideFiles(Mod &CurrentMod) noexcept
{
	// Attempt to find all files in the Sides Dir
	try
	{
		// Setup the template for each side folder 
		for (unsigned int s = 0; s < CurrentMod.Sides.size(); s++)
		{
			if (CurrentMod.Sides[s].SideFolder != "")
			{
				// Search for the actual files for this iteration's side folder
				for (auto& r : fs::recursive_directory_iterator(fs::current_path() / CurrentMod.ModFolder / "Sides" / CurrentMod.Sides[s].SideFolder))
				{
					if (!fs::is_directory(r.path()) && r.path().has_extension())
					{
						if (r.path().extension().string() == ".odf")
						{
							CurrentMod.Sides[s].ODF.emplace_back(std::make_pair(r.path().filename().string().substr(0, r.path().filename().string().length() - 4), r.path().string()));
						}
						else if (r.path().extension().string() == ".msh")
						{
							CurrentMod.Sides[s].MSH.emplace_back(std::make_pair(r.path().filename().string().substr(0, r.path().filename().string().length() - 4), r.path().string()));
						}
						else if (r.path().extension().string() == ".tga")
						{
							CurrentMod.Sides[s].TGA.emplace_back(std::make_pair(r.path().filename().string().substr(0, r.path().filename().string().length() - 4), r.path().string()));
						}
						else if (r.path().extension().string() == ".fx")
						{
							CurrentMod.Sides[s].FX.emplace_back(std::make_pair(r.path().filename().string().substr(0, r.path().filename().string().length() - 3), r.path().string()));
						}
						else if (r.path().extension().string() == ".req")
						{
							CurrentMod.Sides[s].REQ.emplace_back(std::make_pair(r.path().filename().string().substr(0, r.path().filename().string().length() - 4), r.path().string()));
						}
					}
				}
			}
		}
	}
	catch (...)
	{
		;
	}
}

void addAddmeFiles(Mod &CurrentMod) noexcept
{
	// Attempt to find the addme.lua in the addme dir
	try
	{
		if (CurrentMod.Addme.first == "" && CurrentMod.Addme.second == "")
		{
			for (auto& r : fs::recursive_directory_iterator(fs::current_path() / CurrentMod.ModFolder / "addme"))
				if (!fs::is_directory(r.path()) && r.path().has_extension())
					if (r.path().extension().string() == ".lua")
					{
						CurrentMod.Addme = (std::make_pair(r.path().filename().string().substr(0, r.path().filename().string().length() - 4), r.path().string()));
					}
		}
	}
	catch (...)
	{
		;
	}
}

namespace Project2 {

	using namespace msclr::interop;
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm()
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::SplitContainer^ splitContainer1;
	public: System::Windows::Forms::TreeView^ treeView1;
	private: System::Windows::Forms::RichTextBox^ richTextBox1;
	private: System::Windows::Forms::Label^ label1;



	private: System::Windows::Forms::Button^ button2;




	public:


	public:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->treeView1 = (gcnew System::Windows::Forms::TreeView());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->richTextBox1 = (gcnew System::Windows::Forms::RichTextBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer1))->BeginInit();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			this->SuspendLayout();
			// 
			// splitContainer1
			// 
			this->splitContainer1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->splitContainer1->Location = System::Drawing::Point(0, 0);
			this->splitContainer1->Name = L"splitContainer1";
			// 
			// splitContainer1.Panel1
			// 
			this->splitContainer1->Panel1->Controls->Add(this->treeView1);
			// 
			// splitContainer1.Panel2
			// 
			this->splitContainer1->Panel2->Controls->Add(this->button2);
			this->splitContainer1->Panel2->Controls->Add(this->label1);
			this->splitContainer1->Panel2->Controls->Add(this->richTextBox1);
			this->splitContainer1->Size = System::Drawing::Size(770, 563);
			this->splitContainer1->SplitterDistance = 254;
			this->splitContainer1->TabIndex = 0;
			// 
			// treeView1
			// 
			this->treeView1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->treeView1->LabelEdit = true;
			this->treeView1->Location = System::Drawing::Point(12, 12);
			this->treeView1->Name = L"treeView1";
			this->treeView1->ShowNodeToolTips = true;
			this->treeView1->Size = System::Drawing::Size(239, 539);
			this->treeView1->TabIndex = 0;
			this->treeView1->TabStop = false;
			this->treeView1->AfterLabelEdit += gcnew System::Windows::Forms::NodeLabelEditEventHandler(this, &MyForm::TreeView1_AfterLabelEdit);
			this->treeView1->NodeMouseClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &MyForm::treeView1_NodeMouseClick);
			this->treeView1->NodeMouseDoubleClick += gcnew System::Windows::Forms::TreeNodeMouseClickEventHandler(this, &MyForm::TreeView1_NodeMouseDoubleClick);
			// 
			// button2
			// 
			this->button2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->button2->Location = System::Drawing::Point(317, 194);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(181, 30);
			this->button2->TabIndex = 5;
			this->button2->Text = L"Save Changes";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &MyForm::button2_Click);
			// 
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(3, 200);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(91, 24);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Contents";
			// 
			// richTextBox1
			// 
			this->richTextBox1->AcceptsTab = true;
			this->richTextBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->richTextBox1->DetectUrls = false;
			this->richTextBox1->EnableAutoDragDrop = true;
			this->richTextBox1->HideSelection = false;
			this->richTextBox1->Location = System::Drawing::Point(3, 227);
			this->richTextBox1->Name = L"richTextBox1";
			this->richTextBox1->ScrollBars = System::Windows::Forms::RichTextBoxScrollBars::ForcedBoth;
			this->richTextBox1->ShowSelectionMargin = true;
			this->richTextBox1->Size = System::Drawing::Size(495, 324);
			this->richTextBox1->TabIndex = 0;
			this->richTextBox1->Text = L"";
			this->richTextBox1->WordWrap = false;
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(12, 24);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoScroll = true;
			this->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->ClientSize = System::Drawing::Size(770, 563);
			this->Controls->Add(this->splitContainer1);
			this->Font = (gcnew System::Drawing::Font(L"Georgia", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->HelpButton = true;
			this->Margin = System::Windows::Forms::Padding(4);
			this->MinimumSize = System::Drawing::Size(786, 602);
			this->Name = L"MyForm";
			this->ShowIcon = false;
			this->SizeGripStyle = System::Windows::Forms::SizeGripStyle::Show;
			this->Text = L"Cal\'s ModTool";
			this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
			this->splitContainer1->Panel1->ResumeLayout(false);
			this->splitContainer1->Panel2->ResumeLayout(false);
			this->splitContainer1->Panel2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->splitContainer1))->EndInit();
			this->splitContainer1->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion

		// Code that will be run on startup
	public: System::Void MyForm_Load(System::Object^ sender, System::EventArgs^ e)
	{
		std::string Temppath = HOMEPATH.relative_path().string();
		std::string Pathstr = HOMEPATH.relative_path().string();
		for (unsigned short i = 0; i < Temppath.length(); ++i)
			Pathstr[i] = std::tolower(Temppath[i]);

		if (Pathstr != "bf2_modtools")
		{
			// Let the user know the program is in the wrong place
			MessageBox::Show("Incorrect program location! \nPlease place this program in your BF2_ModTools folder.\nClick OK to exit.", "Error!");
			exit(1); // Exit with error code 1
		}
		else
		{
			addDataFolders();
			addSideFolders();
			PostLoad();
		}

		// End of startup code
	}

	public: void PostLoad()
	{
		// Actual function that indexes the files
		//TODO: Perhaps save a template so first startup is slow, res are quick?!
		this->treeView1->BeginUpdate();
		for (unsigned int c = 0; c < ModFolders.size(); c++)
		{
			// First do the file indexing manually
			addWorldFiles(ModFolders[c]);
			addSideFiles(ModFolders[c]);
			addCommonFiles(ModFolders[c]);
			addAddmeFiles(ModFolders[c]);
		}

		// Setup tree hierarchy for each data folder
		for (unsigned int c = 0; c < ModFolders.size(); c++)
		{
			// Add the template hierarchy
			String^ SysStr = gcnew String(ModFolders[c].ModFolder.c_str());
			this->treeView1->Nodes->Add("NULLKEY", SysStr);
			this->treeView1->Nodes[c]->Nodes->Add("NULLKEY", "Worlds");
			this->treeView1->Nodes[c]->Nodes->Add("NULLKEY", "Sides");
			this->treeView1->Nodes[c]->Nodes->Add("NULLKEY", "Common");

			if (fs::exists(fs::current_path() / ModFolders[c].ModFolder / "Worlds"))
			{
				this->treeView1->Nodes[c]->Nodes[0]->Nodes->Add("NULLKEY", "ODF");
				this->treeView1->Nodes[c]->Nodes[0]->Nodes->Add("NULLKEY", "MSH");
				this->treeView1->Nodes[c]->Nodes[0]->Nodes->Add("NULLKEY", "TGA");
				this->treeView1->Nodes[c]->Nodes[0]->Nodes->Add("NULLKEY", "EFFECTS");
				this->treeView1->Nodes[c]->Nodes[0]->Nodes->Add("NULLKEY", "REQ");
			}

			if (fs::exists(fs::current_path() / ModFolders[c].ModFolder / "Common"))
			{
				this->treeView1->Nodes[c]->Nodes[2]->Nodes->Add("NULLKEY", "ODF");
				this->treeView1->Nodes[c]->Nodes[2]->Nodes->Add("NULLKEY", "MSH");
				this->treeView1->Nodes[c]->Nodes[2]->Nodes->Add("NULLKEY", "TGA");
				this->treeView1->Nodes[c]->Nodes[2]->Nodes->Add("NULLKEY", "SCRIPTS");
				this->treeView1->Nodes[c]->Nodes[2]->Nodes->Add("NULLKEY", "EFFECTS");
				this->treeView1->Nodes[c]->Nodes[2]->Nodes->Add("NULLKEY", "REQ");
			}

			if (fs::exists(fs::current_path() / ModFolders[c].ModFolder / "_BUILD" / "Modtools VisualMunge.exe"))
				this->treeView1->Nodes[c]->Nodes->Add((SysStr + "\\_BUILD\\Modtools VisualMunge.exe"), "Munger");

			// Attempt to find all files in the Worlds Dir
			// Always use try{}Catch(...){} in case a folder is missing 
			try
			{
				if (fs::exists(fs::current_path() / ModFolders[c].ModFolder / "Worlds"))
				{
					//Add ODF nodes
					if (!ModFolders[c].Worlds[0].ODF.empty())
						for (int d = 0; d < ModFolders[c].Worlds[0].ODF.size(); d++)
						{
							String^ SysName = gcnew String(ModFolders[c].Worlds[0].ODF[d].first.c_str());
							String^ SysFile = gcnew String(ModFolders[c].Worlds[0].ODF[d].second.c_str());
							this->treeView1->Nodes[c]->Nodes[0]->Nodes[0]->Nodes->Add(SysFile, SysName);
						}

					// Add MSH nodes
					if (!ModFolders[c].Worlds[0].MSH.empty())
						for (int d = 0; d < ModFolders[c].Worlds[0].MSH.size(); d++)
						{
							String^ SysName = gcnew String(ModFolders[c].Worlds[0].MSH[d].first.c_str());
							String^ SysFile = gcnew String(ModFolders[c].Worlds[0].MSH[d].second.c_str());
							this->treeView1->Nodes[c]->Nodes[0]->Nodes[1]->Nodes->Add(SysFile, SysName);
						}

					// Add TGA nodes
					if (!ModFolders[c].Worlds[0].TGA.empty())
						for (int d = 0; d < ModFolders[c].Worlds[0].TGA.size(); d++)
						{
							String^ SysName = gcnew String(ModFolders[c].Worlds[0].TGA[d].first.c_str());
							String^ SysFile = gcnew String(ModFolders[c].Worlds[0].TGA[d].second.c_str());
							this->treeView1->Nodes[c]->Nodes[0]->Nodes[2]->Nodes->Add(SysFile, SysName);
						}

					// Add FX nodes
					if (!ModFolders[c].Worlds[0].FX.empty())
						for (int d = 0; d < ModFolders[c].Worlds[0].FX.size(); d++)
						{
							String^ SysName = gcnew String(ModFolders[c].Worlds[0].FX[d].first.c_str());
							String^ SysFile = gcnew String(ModFolders[c].Worlds[0].FX[d].second.c_str());
							this->treeView1->Nodes[c]->Nodes[0]->Nodes[3]->Nodes->Add(SysFile, SysName);
						}

					// Add REQ nodes
					if (!ModFolders[c].Worlds[0].REQ.empty())
						for (int d = 0; d < ModFolders[c].Worlds[0].REQ.size(); d++)
						{
							String^ SysName = gcnew String(ModFolders[c].Worlds[0].REQ[d].first.c_str());
							String^ SysFile = gcnew String(ModFolders[c].Worlds[0].REQ[d].second.c_str());
							this->treeView1->Nodes[c]->Nodes[0]->Nodes[4]->Nodes->Add(SysFile, SysName);
						}
				}
			}
			catch (...)
			{
				;
			}

			// Attempt to find all files in the Sides Dir
			try
			{
				if (fs::exists(fs::current_path() / ModFolders[c].ModFolder / "Sides"))
				{
					// Setup the template for each side folder 
					for (int s = 0; s < ModFolders[c].Sides.size(); s++)
					{
						String^ SysStr = gcnew String(ModFolders[c].Sides[s].SideFolder.c_str());
						this->treeView1->Nodes[c]->Nodes[1]->Nodes->Add("NULLKEY", SysStr);

						this->treeView1->Nodes[c]->Nodes[1]->Nodes[s]->Nodes->Add("NULLKEY", "ODF");
						this->treeView1->Nodes[c]->Nodes[1]->Nodes[s]->Nodes->Add("NULLKEY", "MSH");
						this->treeView1->Nodes[c]->Nodes[1]->Nodes[s]->Nodes->Add("NULLKEY", "TGA");
						this->treeView1->Nodes[c]->Nodes[1]->Nodes[s]->Nodes->Add("NULLKEY", "EFFECTS");
						this->treeView1->Nodes[c]->Nodes[1]->Nodes[s]->Nodes->Add("NULLKEY", "REQ");


						// Search for the actual files for this iteration's side folder
						if (!ModFolders[c].Sides[s].ODF.empty())
							for (unsigned int d = 0; d < ModFolders[c].Sides[s].ODF.size(); d++)
							{
								String^ SysName = gcnew String(ModFolders[c].Sides[s].ODF[d].first.c_str());
								String^ SysFile = gcnew String(ModFolders[c].Sides[s].ODF[d].second.c_str());
								this->treeView1->Nodes[c]->Nodes[1]->Nodes[s]->Nodes[0]->Nodes->Add(SysFile, SysName);
							}

						if (!ModFolders[c].Sides[s].MSH.empty())
							for (unsigned int d = 0; d < ModFolders[c].Sides[s].MSH.size(); d++)
							{
								String^ SysName = gcnew String(ModFolders[c].Sides[s].MSH[d].first.c_str());
								String^ SysFile = gcnew String(ModFolders[c].Sides[s].MSH[d].second.c_str());
								this->treeView1->Nodes[c]->Nodes[1]->Nodes[s]->Nodes[1]->Nodes->Add(SysFile, SysName);
							}

						if (!ModFolders[c].Sides[s].TGA.empty())
							for (unsigned int d = 0; d < ModFolders[c].Sides[s].TGA.size(); d++)
							{
								String^ SysName = gcnew String(ModFolders[c].Sides[s].TGA[d].first.c_str());
								String^ SysFile = gcnew String(ModFolders[c].Sides[s].TGA[d].second.c_str());
								this->treeView1->Nodes[c]->Nodes[1]->Nodes[s]->Nodes[2]->Nodes->Add(SysFile, SysName);
							}

						if (!ModFolders[c].Sides[s].FX.empty())
							for (unsigned int d = 0; d < ModFolders[c].Sides[s].FX.size(); d++)
							{
								String^ SysName = gcnew String(ModFolders[c].Sides[s].FX[d].first.c_str());
								String^ SysFile = gcnew String(ModFolders[c].Sides[s].FX[d].second.c_str());
								this->treeView1->Nodes[c]->Nodes[1]->Nodes[s]->Nodes[3]->Nodes->Add(SysFile, SysName);
							}

						if (!ModFolders[c].Sides[s].REQ.empty())
							for (unsigned int d = 0; d < ModFolders[c].Sides[s].REQ.size(); d++)
							{
								String^ SysName = gcnew String(ModFolders[c].Sides[s].REQ[d].first.c_str());
								String^ SysFile = gcnew String(ModFolders[c].Sides[s].REQ[d].second.c_str());
								this->treeView1->Nodes[c]->Nodes[1]->Nodes[s]->Nodes[4]->Nodes->Add(SysFile, SysName);
							}
					}
				}
			}
			catch (...)
			{
				;
			}

			// Attempt to find all files in the Common Dir
			try
			{
				if (fs::exists(fs::current_path() / ModFolders[c].ModFolder / "Common"))
				{
					if (!ModFolders[c].Common_ODF.empty())
						for (unsigned int d = 0; d < ModFolders[c].Common_ODF.size(); d++)
						{
							String^ SysName = gcnew String(ModFolders[c].Common_ODF[d].first.c_str());
							String^ SysFile = gcnew String(ModFolders[c].Common_ODF[d].second.c_str());
							this->treeView1->Nodes[c]->Nodes[2]->Nodes[0]->Nodes->Add(SysFile, SysName);
						}
					if (!ModFolders[c].Common_MSH.empty())
						for (unsigned int d = 0; d < ModFolders[c].Common_MSH.size(); d++)
						{
							String^ SysName = gcnew String(ModFolders[c].Common_MSH[d].first.c_str());
							String^ SysFile = gcnew String(ModFolders[c].Common_MSH[d].second.c_str());
							this->treeView1->Nodes[c]->Nodes[2]->Nodes[1]->Nodes->Add(SysFile, SysName);
						}
					if (!ModFolders[c].Common_TGA.empty())
						for (unsigned int d = 0; d < ModFolders[c].Common_TGA.size(); d++)
						{
							String^ SysName = gcnew String(ModFolders[c].Common_TGA[d].first.c_str());
							String^ SysFile = gcnew String(ModFolders[c].Common_TGA[d].second.c_str());
							this->treeView1->Nodes[c]->Nodes[2]->Nodes[2]->Nodes->Add(SysFile, SysName);
						}
					if (!ModFolders[c].Common_LUA.empty())
						for (unsigned int d = 0; d < ModFolders[c].Common_LUA.size(); d++)
						{
							String^ SysName = gcnew String(ModFolders[c].Common_LUA[d].first.c_str());
							String^ SysFile = gcnew String(ModFolders[c].Common_LUA[d].second.c_str());
							this->treeView1->Nodes[c]->Nodes[2]->Nodes[3]->Nodes->Add(SysFile, SysName);
						}
					if (!ModFolders[c].Common_FX.empty())
						for (unsigned int d = 0; d < ModFolders[c].Common_FX.size(); d++)
						{
							String^ SysName = gcnew String(ModFolders[c].Common_FX[d].first.c_str());
							String^ SysFile = gcnew String(ModFolders[c].Common_FX[d].second.c_str());
							this->treeView1->Nodes[c]->Nodes[2]->Nodes[4]->Nodes->Add(SysFile, SysName);
						}
					if (!ModFolders[c].Common_REQ.empty())
						for (unsigned int d = 0; d < ModFolders[c].Common_REQ.size(); d++)
						{
							String^ SysName = gcnew String(ModFolders[c].Common_REQ[d].first.c_str());
							String^ SysFile = gcnew String(ModFolders[c].Common_REQ[d].second.c_str());
							this->treeView1->Nodes[c]->Nodes[2]->Nodes[5]->Nodes->Add(SysFile, SysName);
						}
				}
			}
			catch (...)
			{
				;
			}

			// Attempt to find the addme.lua in the addme dir
			try
			{
				if (fs::exists(fs::current_path() / ModFolders[c].ModFolder / "addme") && ModFolders[c].Addme.first != "" && ModFolders[c].Addme.second != "")
				{
					String^ SysName = gcnew String(ModFolders[c].Addme.first.c_str());
					String^ SysFile = gcnew String(ModFolders[c].Addme.second.c_str());
					this->treeView1->Nodes[c]->Nodes->Add(SysFile, SysName);
				}
			}
			catch (...)
			{
				;
			}
		}
		this->treeView1->EndUpdate();
	}

		  // On Node Double Click
	public: System::Void TreeView1_NodeMouseDoubleClick(System::Object^ sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e)
	{
		try
		{
			// When a user double clicks an item, check the key to see if it references a real file
			std::string NodeName = marshal_as<std::string>(e->Node->Name);
			std::string NodeText = marshal_as<std::string>(e->Node->Text);
			if (NodeName != "NULLKEY")
				if (pattern_match(NodeName, "*.msh") || NodeText == "Munger")
				{
					fs::path TempFilePath(NodeName);
					std::string TempPath = TempFilePath.parent_path().string();
					ShellExecuteA(NULL, "open", TempPath.c_str(), NULL, NULL, 1); // Tell Windows to open that file with the default program
				}
				else
					ShellExecuteA(NULL, "open", NodeName.c_str(), NULL, NULL, 1); // Tell Windows to open that file with the default program
		}
		catch (...)
		{
			;
		}
	}

		  // On Node Label Edit
	public: System::Void TreeView1_AfterLabelEdit(System::Object^ sender, System::Windows::Forms::NodeLabelEditEventArgs^ e)
	{
		try
		{
			std::string NodeName = marshal_as<std::string>(e->Node->Name);
			std::string NodeText = marshal_as<std::string>(e->Label);
			if (NodeName != "NULLKEY")
			{
				if (e->Label->IsNullOrEmpty(e->Label) == false && e->Label->IsNullOrWhiteSpace(e->Label) == false)
				{
					fs::path OGFile(NodeName);
					std::string OGFileName = OGFile.string();
					std::string NewFN = OGFile.parent_path().string() + "\\" + NodeText + OGFile.extension().string();
					String^ TempName = gcnew String(NewFN.c_str());
					String^ badchar1 = "/";
					String^ badchar2 = "\\";
					int renameOp = rename(OGFileName.c_str(), NewFN.c_str());
					if (renameOp == 0 && e->Label->Contains(badchar1) == false && e->Label->Contains(badchar2) == false)
					{
						e->Node->EndEdit(false);
						e->Node->Name = TempName;
					}
					else
						e->CancelEdit = true;
				}
				else
					e->CancelEdit = true;
			}
			else
				e->CancelEdit = true;
		}
		catch (...)
		{
			;
		}
	}

	private: System::Void treeView1_NodeMouseClick(System::Object^ sender, System::Windows::Forms::TreeNodeMouseClickEventArgs^ e)
	{
		try
		{
			std::string NodeName = marshal_as<std::string>(e->Node->Name);
			if (NodeName != "NULLKEY")
			{
				if (pattern_match(NodeName, "*.odf") || pattern_match(NodeName, "*.req") || pattern_match(NodeName, "*.fx") || pattern_match(NodeName, "*.lua"))
				{
					std::ifstream InFile;
					InFile.open(NodeName);
					if (InFile.is_open())
					{
						std::string FileStr;
						InFile.seekg(0, std::ios::end);
						FileStr.reserve(InFile.tellg());
						InFile.seekg(0, std::ios::beg);
						FileStr.assign((std::istreambuf_iterator<char>(InFile)), std::istreambuf_iterator<char>());
						InFile.close();

						String^ FileContents = gcnew String(FileStr.c_str());
						this->richTextBox1->Text = FileContents;
					}
					else
					{
						MessageBox::Show("Unable to read file!", "IO Error!");
					}
				}
			}
			else
			{
				this->richTextBox1->Clear();
			}
		}
		catch (...)
		{
			;
		}
	}
	private: System::Void button2_Click(System::Object^ sender, System::EventArgs^ e)
	{
		try 
		{
			if (this->treeView1->SelectedNode->Name != "NULLKEY")
			{
				std::string NodeName = marshal_as<std::string>(this->treeView1->SelectedNode->Name);
				std::ofstream OutFile;
				OutFile.open(NodeName);
				if (OutFile.is_open())
				{
					std::string EditBoxText = marshal_as<std::string>(this->richTextBox1->Text);
					OutFile << EditBoxText;
					OutFile.close();
				}
			}
		}
		catch (...)
		{
			;
		}
	}
	};
}