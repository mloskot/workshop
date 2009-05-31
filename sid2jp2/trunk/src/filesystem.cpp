// $Id$
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "filesystem.h"
// std
#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include <cassert>
// winstl
#include <winstl/findfile_sequence.hpp>
#include <winstl/directory_functions.hpp>
#include <winstl/filesystem_traits.hpp>
#include <winstl/path.hpp>

namespace sid2jp2 { namespace fs {

void generate_dataset_list(std::string const& input_dir,
                           std::vector<std::string> const& input_list,
                           std::string const& output_dir,                           
                           std::vector<std::pair<std::string, std::string> >& output_list,
                           std::string const& output_ext,
                           bool recurse)
{
    assert(!input_dir.empty());
    assert(!output_dir.empty());

    typedef std::vector<std::string>::const_iterator iterator_t;

    std::string part;
    std::string output_file;
    std::string tmp_dir(output_dir);
    
    if (tmp_dir[tmp_dir.size() - 1] != '\\')
    {
        tmp_dir += '\\';
    }

    assert(tmp_dir[tmp_dir.size() - 1] == '\\');

    for (iterator_t it = input_list.begin(); it != input_list.end(); ++it)
    {
        std::string const& file = (*it);

        // Strip file or path+file
        if (recurse)
            part = file.substr(input_dir.size() + 1);
        else
            part = file.substr(file.rfind('\\') + 1, file.size() - 1);

        // Change file extension
        part.replace(part.find('.'), std::string::npos, output_ext);

        // Compose target path
        output_file.assign(tmp_dir + part);
        output_list.push_back(std::make_pair(file, output_file));
    }
}

bool create_file_path_recurse(std::string const& file_path)
{
    std::string base_path(file_path.substr(0, file_path.rfind('\\')));

    return winstl::create_directory_recurse(base_path.c_str());
}

} // namespace fs 
} // namespace sid2jp2
