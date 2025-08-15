#pragma once

#include <string>

namespace string
{

/**
 * Replaces all occurrences of the given search string in the subject
 * with the given replacement, in-place.
 */
inline void replace_all(std::string& subject, const std::string& search, const std::string& replacement)
{
	if (search.empty()) return; // nothing to do

	std::size_t pos = 0;

	while ((pos = subject.find(search, pos)) != std::string::npos)
	{
		subject.replace(pos, search.length(), replacement);
		pos += replacement.length();
	}
}

/**
 * Replaces all occurrences of of the given search string with
 * the given replacement and returns a new string instance
 * containing the result. The incoming subject is passed by value such
 * that the original string is not altered.
 */
inline std::string replace_all_copy(std::string subject, const std::string& search, const std::string& replacement)
{
	replace_all(subject, search, replacement);
	return subject;
}

/**
* Replaces the first occurrence of the given search string in the subject
* with the given replacement, in-place.
*/
inline void replace_first(std::string& subject, const std::string& search, const std::string& replacement)
{
	if (search.empty()) return; // nothing to do

	std::size_t pos = subject.find(search);

	if (pos != std::string::npos)
	{
		subject.replace(pos, search.length(), replacement);
	}
}

/**
* Replaces the last occurrence of the given search string in the subject
* with the given replacement, in-place.
*/
inline void replace_last(std::string& subject, const std::string& search, const std::string& replacement)
{
	if (search.empty()) return; // nothing to do

	std::size_t pos = subject.rfind(search);

	if (pos != std::string::npos)
	{
		subject.replace(pos, search.length(), replacement);
	}
}

}
