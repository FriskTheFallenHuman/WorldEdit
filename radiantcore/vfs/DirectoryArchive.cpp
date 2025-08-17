#include "DirectoryArchive.h"

#include "gamelib.h"
#include "UnixPath.h"
#include "os/file.h"
#include "os/dir.h"
#include "os/fs.h"
#include "string/encoding.h"
#include <vector>

#include "DirectoryArchiveFile.h"
#include "DirectoryArchiveTextFile.h"

DirectoryArchive::DirectoryArchive(const std::string& root) :
	_root(root)
{}

ArchiveFilePtr DirectoryArchive::openFile(const std::string& name) 
{
	UnixPath path(_root);
	path.push_filename(name);

	std::shared_ptr<archive::DirectoryArchiveFile> file = 
		std::make_shared<archive::DirectoryArchiveFile>(name, path);

	if (!file->failed())
	{
		return file;
	}

	return ArchiveFilePtr();
}

ArchiveTextFilePtr DirectoryArchive::openTextFile(const std::string& name)
{
	UnixPath path(_root);
	path.push_filename(name);

	auto file = std::make_shared<archive::DirectoryArchiveTextFile>(name, _root, path);

	if (!file->failed()) 
	{
		return file;
	}

	return ArchiveTextFilePtr();
}

bool DirectoryArchive::containsFile(const std::string& name)
{
	UnixPath path(_root);
	std::string filePath = std::string(path) + name;
	return os::fileIsReadable(filePath);
}

void DirectoryArchive::traverse(Visitor& visitor, const std::string& root)
{
	// Initialise the search's starting point
	fs::path start(_root + root);

	if (!fs::exists(start))
	{
		return;
	}

	// For cutting off the base path
	std::size_t rootLen = _root.length();

	for (fs::recursive_directory_iterator it(start); it != fs::recursive_directory_iterator(); ++it)
	{
		// Get the candidate
		const fs::path& candidate = *it;

		try
		{
			auto candidateStr = candidate.generic_string();

			if (fs::is_directory(candidate))
			{
				// Check if we should traverse further
				if (visitor.visitDirectory(candidateStr.substr(rootLen), os::getDepth(it) + 1))
				{
					// Visitor returned true, prevent going deeper into it
					os::disableRecursionPending(it);
				}
			}
			else
			{
				// File
				visitor.visitFile(candidateStr.substr(rootLen), *this);
			}
		}
		catch (const std::system_error& ex)
		{
			rWarning() << "[vfs] Skipping file " << string::unicode_to_utf8(candidate.filename().wstring()) <<
				" - possibly unsupported characters in filename? " <<
				"(Exception: " << ex.what() << ")" << std::endl;
		}
	}
}

std::size_t DirectoryArchive::getFileSize(const std::string& relativePath)
{
	UnixPath path(_root);
	return os::getFileSize(std::string(path) + relativePath);
}

bool DirectoryArchive::getIsPhysical(const std::string& relativePath)
{
	// this whole class represents a physical directory, we don't even check
	return true;
}

std::string DirectoryArchive::getArchivePath(const std::string& relativePath)
{
	return _root;
}