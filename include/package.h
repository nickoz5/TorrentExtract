#pragma once

class package
{
public:
	package();
	~package();

public:
	// process this package
	bool process();

private:
	// scan the source path for artefacts
	void scan_directory(const std::string& source);

	// process the given archive
	void process_archive(const std::string& filepath);

	// process the given payload
	bool process_payload(const std::string& filepath);

	bool is_payload(const std::string& filename);
	bool is_archive_file(const std::string& filename);

	bool extract_archive(const std::string& filepath, const std::string& destpath);

private:
	// list of known video file extensions
	static const char * s_video_files_ext[];

	// list of known archive file extensions
	static const char * s_archive_files_ext[];
};

