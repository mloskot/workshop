// $Id$
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
#ifndef SID2JP2_ERROR_H_INCLUDED
#define SID2JP2_ERROR_H_INCLUDED

#include <string>

namespace sid2jp2 { namespace error {

void file_translation_failure(HWND hParent, std::string const& file);

} // namespace error
} // namespace sid2jp2

#endif // SID2JP2_ERROR_H_INCLUDED