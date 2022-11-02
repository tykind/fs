/*
*	Desc: File system helper
*	Note: This is a neat wrapper around C++'s file system
*/
#pragma once
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <typeinfo>
#include <memory>

#define BASE_ID const std::type_info &type = typeid(*this)
#define GET_FILE_TYPE virtual enums::FileTypes getFileType() { return fileType; }

namespace fs
{
	namespace enums
	{
		enum FileTypes
		{
			FILE,
			DOCUMENT,
			FOLDER
		};
	}

	struct File
	{
		std::filesystem::path path;
		enums::FileTypes fileType{ enums::FILE };
		bool exists{ false };

		GET_FILE_TYPE

			// Translate type to another

			template<typename _Ty>
		std::shared_ptr<_Ty> translate()
		{
			return std::make_shared<_Ty>(dynamic_cast<_Ty&>(*this));
		}

		// Just sets path with some extra information

		File(std::filesystem::path _path) : path{ _path }
		{
			if (std::filesystem::exists(path))
			{
				exists = true;
			}
		}
	};

	template <class _Dt>
	concept FileNode = std::is_class_v<_Dt> && std::derived_from<_Dt, File>;

	template <FileNode _Node, class... _Args>
	inline auto makeFile(_Args... args)
	{
		return std::make_shared<_Node>(args...);
	}

	struct Document final : public File
	{
		using File::File;
		enums::FileTypes fileType{ enums::DOCUMENT };

		BASE_ID;
		GET_FILE_TYPE

			// Read files content

			auto read()
		{
			std::stringstream fileContent;
			std::ifstream file(path.c_str(), std::ios::ate | std::ios::binary);

			file.seekg(0, std::ios::beg);
			fileContent << file.rdbuf();
			file.close();

			return fileContent;
		}

		// Writes to file

		void write(std::stringstream& data)
		{
			std::ofstream file(path.c_str(), std::ios::ate | std::ios::binary);

			data >> file.rdbuf();
			file.close();
		}
	};

	struct Folder final : public File
	{
		using File::File;
		std::vector<std::shared_ptr<File>> contents;
		enums::FileTypes fileType{ enums::FOLDER };

		BASE_ID;
		GET_FILE_TYPE

			// Read directory and deduct folder/document

			std::vector<std::shared_ptr<File>>& read()
		{
			contents.clear();
			for (auto const& entry : std::filesystem::directory_iterator{ path })
			{
				if (entry.is_directory())
					contents.push_back(makeFile<Folder>(entry.path()));
				else
					contents.push_back(makeFile<Document>(entry.path()));
			}

			return contents;
		}
	};
}

#undef BASE_ID
#undef GET_FILE_TYPE
