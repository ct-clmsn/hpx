//  Copyright (c) 2005-2007 Andre Merzky
//  Copyright (c) 2005-2011 Hartmut Kaiser
//  Copyright (c)      2011 Bryce Lelbach
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_UTIL_SECTION_SEP_17_2008_022PM)
#define HPX_UTIL_SECTION_SEP_17_2008_022PM

#include <map>
#include <iosfwd>

#include <boost/lexical_cast.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>

// suppress warnings about dependent classes not being exported from the dll
#if defined(BOOST_MSVC)
#pragma warning(push)
#pragma warning(disable: 4091 4251 4231 4275 4660)
#endif

///////////////////////////////////////////////////////////////////////////////
//  section serialization format version
#define HPX_SECTION_VERSION 0x10

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace util
{
    ///////////////////////////////////////////////////////////////////////////
    class HPX_EXPORT section
    {
    public:
        typedef std::map<std::string, std::string> entry_map;
        typedef std::map<std::string, section> section_map;

    private:
        section *this_() { return this; }

        section* root_;
        entry_map entries_;
        // FIXME: section_env_ is filled, but doesn't appear to be used anywhere
        entry_map section_env_;
        section_map sections_;
        std::string name_;

    private:
        friend class boost::serialization::access;

        template <typename Archive>
        void save(Archive& ar, const unsigned int version) const;

        template <typename Archive>
        void load(Archive& ar, const unsigned int version);

        BOOST_SERIALIZATION_SPLIT_MEMBER()

    protected:
        bool regex_init();
        void line_msg(std::string const& msg, std::string const& file,
            int lnum = 0);

    public:
        section();
        explicit section(std::string const& filename, section* root = NULL);
        section(section const& in);
        ~section() {}

        section& operator=(section const& rhs);

        void parse(std::string const& sourcename,
            std::vector<std::string> const& lines);

        void parse(std::string const& sourcename,
            std::string const& line)
        {
            std::vector<std::string> lines;
            lines.push_back(line);
            parse(sourcename, lines);
        }

        void read(std::string const& filename);
        void merge(std::string const& second);
        void merge(section& second);
        void dump(int ind = 0, std::ostream& strm = std::cout) const;

        void add_section(std::string const& sec_name, section& sec,
            section* root = NULL);
        bool has_section(std::string const& sec_name) const;

        section* get_section (std::string const& sec_name);
        section const* get_section (std::string const& sec_name) const;

        section_map const& get_sections() const
            { return sections_; }

        void add_entry(std::string const& key, std::string const& val);
        bool has_entry(std::string const& key) const;
        std::string get_entry(std::string const& key) const;
        std::string get_entry(std::string const& key, std::string const& dflt) const;
        template <typename T>
        std::string get_entry(std::string const& key, T dflt) const
        {
            return get_entry(key, boost::lexical_cast<std::string>(dflt));
        }

        entry_map const& get_entries() const
            { return entries_; }
        std::string expand(std::string in) const;

        void expand(std::string&, std::string::size_type) const;
        void expand_bracket(std::string&, std::string::size_type) const;
        void expand_brace(std::string&, std::string::size_type) const;

        void set_root(section* r, bool recursive = false)
        {
            root_ = r;
            if (recursive) {
                section_map::iterator send = sections_.end();
                for (section_map::iterator si = sections_.begin(); si != send; ++si)
                    si->second.set_root(r, true);
            }
        }
        section* get_root() const { return root_; }
        std::string get_name() const { return name_; }
        void set_name(std::string const& name) { name_ = name; }

        section clone(section* root = NULL) const;
    };

}} // namespace hpx::util

///////////////////////////////////////////////////////////////////////////////
// this is the current version of the parcel serialization format
// this definition needs to be in the global namespace
BOOST_CLASS_VERSION(hpx::util::section, HPX_SECTION_VERSION)

#endif

