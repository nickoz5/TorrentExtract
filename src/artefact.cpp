#include "main.h"
#include "artefact.h"
#include "fileio.h"

//#include <stdlib.h>
//#include <memory>


const std::string artefact::s_packagetype_tags[] =
{
	"hdtv", "web-dl", "pdtv", "dsr", "web", "webrip", "bluray",
	"bdrip", "dts", "blu-ray", "webscr", "dvdrip", "web-dl"
};

const std::string artefact::s_quality_tags[] =
{
	"720p", "1080p", "hdtv", "limited", "webscr", "proper"
};


artefact::artefact(const std::string& filepath)
{
	m_filepath = filepath;
	m_type = TYPE_UNKNOWN;
	m_episode_no = 0;
	m_season_no = 0;
	m_year = 0;
}

bool artefact::parse()
{
	// find show name
	std::string title;
	std::string title_word;

	std::string filename = fileio(m_filepath).get_filename();
	to_lower(filename);
	
	if (!is_valid_artefact())
		return false;

	// is TV show..
	for (size_t i = 0; i < filename.length(); i++)
	{
		char ch = filename[i];

		if (ch == '.' || ch == ' ' || ch == '_' || i == filename.length() - 1)
		{
			// process this word..
				
			// look for tags that indicate the title has ended.
			if (is_quality_tag(title_word))
				break;

			// check for season/episode id
			if (find_seasonep_id(title_word, i))
			{
				m_type = TYPE_TV_SERIES;
				break;
			}

			// check for a year/date episode name
			if (find_movieyear_id(title_word))
			{
				m_type = TYPE_MOVIE;
				break;
			}

			// nothing special, assume its part of the title.
			// add this word to the title string.
			if (!title.empty()) title += " ";
			title += title_word;

			// start the next word..
			title_word.clear();

			// skip '.' char
			continue;
		}

		title_word += ch;
	}

	if (title.empty())
	{
		return false;
	}

	m_title = to_title_case(title);
	
	return true;
}

bool artefact::find_movieyear_id(const std::string& title_word)
{
	if (title_word.length() == 4)
	{
		if (is_number(title_word[0]) &&
			is_number(title_word[1]) &&
			is_number(title_word[2]) &&
			is_number(title_word[3]))
		{
			int iYear = atoi(title_word.c_str());
			if (iYear > 1970 && iYear < 2020) {
				// found movie year
				m_year = iYear;
				return true;
			}
		}
	}

	return false;
}

bool artefact::is_number(const char c)
{
	return c >= '0' && c <= '9';
}

bool artefact::find_seasonep_id(const std::string& title_word, size_t pos)
{
	std::string filename = fileio(m_filepath).get_filename();

	// check S00E00
	if (title_word.length() >= 6 && (title_word[0] == 'S' || title_word[0] == 's') && (title_word[3] == 'E' || title_word[3] == 'e'))
	{
		if (is_number(title_word[1]) && is_number(title_word[2]) &&
			is_number(title_word[4]) && is_number(title_word[5]))
		{
			// found season/episode number
			m_season_no = atoi(title_word.c_str() + 1);
			m_episode_no = atoi(title_word.c_str() + 4);
			return true;
		}
	}

	// check S00
	if (title_word.length() >= 3 && (title_word[0] == 'S' || title_word[0] == 's'))
	{
		if (is_number(title_word[1]) && is_number(title_word[2]))
		{
			// found season/episode number
			m_season_no = atoi(title_word.c_str() + 1);
			m_episode_no = 0;
			return true;
		}
	}

	// check 00x00
	if (title_word.length() == 5 && (title_word[2] == 'X' || title_word[2] == 'x'))
	{
		if (is_number(title_word[0]) && is_number(title_word[1]) &&
			is_number(title_word[3]) && is_number(title_word[4]))
		{
			// found season/episode number
			m_season_no = atoi(title_word.c_str() + 0);
			m_episode_no = atoi(title_word.c_str() + 3);
			return true;
		}
	}

	// check for date format season/episode
	if (title_word.length() == 4 &&
		title_word[0] >= '1' && title_word[0] <= '2' &&
		(title_word[1] == '9' || title_word[1] == '0') &&
		is_number(title_word[2]) && is_number(title_word[3]))
	{
		if (pos + 8 < filename.length())
		{
			char szDate[7];
			for (size_t i2 = pos; i2 <= pos + 6; i2++)
				szDate[i2 - pos] = filename[i2];

			// find a date..
			if ((szDate[0] == '.' || szDate[0] == ' ' || szDate[0] == '_' || szDate[0] == '/') &&
				szDate[1] >= '0' && szDate[1] <= '9' && szDate[2] >= '0' && szDate[2] <= '9' &&
				(szDate[3] == '.' || szDate[3] == ' ' || szDate[3] == '_' || szDate[3] == '/') &&
				szDate[4] >= '0' && szDate[4] <= '9' && szDate[5] >= '0' && szDate[5] <= '9')
			{
				// found season/episode number
				m_season_no = -1;
				m_episode_no = -1;
				return true;
			}
		}
	}

	return false;
}

bool artefact::is_valid_artefact()
{
	// find known package tags
	for (int i = 0; i < sizeof(s_packagetype_tags) / sizeof(std::string); i++)
	{
		size_t pos = m_title.find(s_packagetype_tags[i]);
		if (pos != m_title.npos)
			return true;
	}
	return false;
}

bool artefact::is_quality_tag(const std::string& word)
{
	bool found_tag = false;
	for (int i = 0; i < sizeof(s_quality_tags) / sizeof(char*); i++)
	{
		size_t pos = m_title.find(s_quality_tags[i]);
		if (pos != m_title.npos)
			return true;
	}
	return found_tag;
}