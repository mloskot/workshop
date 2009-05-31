// $Id$
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
#ifndef SID2JP2_FILESYSTEM_H_INCLUDED
#define SID2JP2_FILESYSTEM_H_INCLUDED

#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include <cassert>
#include <winstl/findfile_sequence.hpp>
#include <winstl/directory_functions.hpp>
#include <winstl/filesystem_traits.hpp>
#include <winstl/path.hpp>

namespace sid2jp2 { namespace fs {

template <typename Seq, typename Cont>
void collect_files(std::string const& dir, std::string const& ext, Cont& out)
{
    typedef typename Seq sequence_t;
    typedef std::back_insert_iterator<Cont> back_iterator_t;

    // Build collection of files in 'dir'
    sequence_t files(dir.c_str(), ext.c_str(), sequence_t::files);

    back_iterator_t back_iter(out);

    // Collect files to a container
    for (sequence_t::const_iterator it = files.begin();
        it != files.end(); ++back_iter, ++it)
    {
        *back_iter= static_cast<char const*>(*it);
    }
}

template <typename Cont>
void generate_file_list(std::string const& dir,
                        std::string const& ext,
                        Cont& out)
{
    typedef winstl::findfile_sequence_a sequence_t;
    collect_files<sequence_t>(dir, ext, out);
}

template <typename Cont>
void generate_file_list_recurse(std::string const& basedir,
                                std::string const& ext,
                                Cont& out)
{
    typedef winstl::findfile_sequence_a sequence_t;
    typedef sequence_t::value_type value_t;

    // Build collection of directories in 'basedir'
    sequence_t dirs(basedir.c_str(), "*", sequence_t::directories);

    // Build collection of files, recursively
    for (sequence_t::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
    {
        value_t const& current = (*it);
        collect_files<sequence_t>(current.get_path(), ext, out);
        
        // Process next directory
        generate_file_list_recurse(current.get_path(), ext, out);
    }
}

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
            part = file.substr(tmp_dir.size() - 1);
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

#endif // SID2JP2_FILESYSTEM_H_INCLUDED