#ifndef _StringMatch_h_
#define _StringMatch_h_

#include <vector>
#include <string>


namespace StringMatch {

//! ***Patten matcher***
//!
//! glob patterns:
//! *        matches zero or more characters
//! ?        matches any single character
//! [set]    matches any character in the set
//! [^set]   matches any character NOT in the set where a set is a group of characters
//!          or ranges. a range is written as two characters seperated with a hyphen:
//!          s-z denotes all characters between a to z inclusive.
//! [-set]   set matches a literal hypen and any character in the set
//! []set]   matches a literal close bracket and any character in the set
//!
//!  char    matches itself except where char is '*' or '?' or '['
//! \char    matches char, including any pattern character
//!
//! Examples:
//!    a*c        ac abc abbc ...
//!    a?c        acc abc aXc ...
//!    a[a-z]c    aac abc acc ...
//!    a[-a-z]c   a-c aac abc ...
//

///
/// \brief match matches a pattern to a given string
/// \param patten
/// \param str string to be matched
/// \return true, if match applies, faslse otherwhile
bool match(const std::string& pattern, const std::string& str);

///
/// \brief match matches a list of patterns to a given string
/// \param patternList list of pattens
/// \param str string to me matched to
/// \return true, if any of the patterns maches at string. false otherwise.
bool match(const std::vector<std::string>& patternList, const std::string& str);


//Note:
// Code collected from: http://www.cse.yorku.ca/~oz/
// robust glob pattern matcher
// ozan s. yigit/dec 1994
// public domain

}

bool testStringMatch(const int argc, const char** argv);

#endif
