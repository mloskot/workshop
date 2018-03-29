#!/usr/bin/perl
#
# colorbb.pl 0.2
#
# Copyright: (C) 2009, Mateusz Loskot <mateusz@loskot.net>
#
# colorbb is based on colormake 0.3 written by Bjarni R. Einarsson.
#
# Copyright: (C) 1999, Bjarni R. Einarsson <bre@netverjar.is>
#                      http://bre.klaki.net/
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# Some useful color codes, see end of file for more.
$col_background =   "\033[07m";
$col_blink =        "\033[05m";
$col_brighten =     "\033[01m";
$col_brown =        "\033[33m";
$col_cyan =         "\033[36m";
$col_green =        "\033[32m";
$col_ltgray =       "\033[37m";
$col_norm =         "\033[00m";
$col_purple =       "\033[35m";
$col_red =          "\033[31m";
$col_underline =    "\033[04m";

# Customize colors here...
#
$col_default =      $col_ltgray;
$col_bjam =         $col_ltgray;
$col_compile =      $col_purple . $col_brighten;
$col_link =         $col_purple . $col_brighten;
$col_test =         $col_purple . $col_brighten;
$col_filename =     $col_brown;
$col_linenum =      $col_cyan;
$col_trace =        $col_brown;
$col_warning =      $col_green;
$tag_error =        "";
$col_error =        $tag_error . $col_red . $col_brighten;
$error_highlight =  $col_brighten;

# Get size of terminal
#
$lines = shift @ARGV || 0;
$cols  = shift @ARGV || 0;
$cols -= 19;

$in = 'unknown';
$| = 1;
while (<>)
{
    $orgline = $thisline = $_;

    # Remove multiple spaces
    $thisline =~ s/  \+/ /g;

    # Truncate lines.
    # I suppose this is bad, but it's better than what less does!
    if ($cols >= 0)
    {
        $thisline =~ s/^(.{$cols}).....*(.{15})$/$1 .. $2/;
    }

    if ($thisline =~ s/^(.+(failed\s).*)$/$col_error$1$col_norm/)
    {
        $in = $1;
    }
    elsif ($thisline =~ s/^(.*(compile).*)$/$col_compile$1$col_norm/)
    {
        $in = 'compile';
    }
    elsif ($thisline =~ s/^(.*(\blink).*)$/$col_link$1$col_norm/)
    {
        $in = 'link';
    }
    elsif ($thisline =~ s/^(.*(testing).*)$/$col_test$1$col_norm/)
    {
        $in = 'test';
    }
    elsif ($thisline =~ s/^(.*(skipped).*)$/$col_bjam$1$col_norm/)
    {
        $in = 'bjam';
    }
    elsif ($thisline =~ /^(\s*\(|\[|a(r|wk)|c(p|d|h(mod|own))|do(ne)?|e(cho|lse)|f(ind|or)|i(f|nstall)|mv|perl|r(anlib|m(dir)?)|s(e(d|t)|trip)|tar)\s+/)
    {
        $in = $1;
    }
    elsif ($in eq 'compile')
    {
        # Do interesting things if make is compiling something.
        if (($thisline !~ /[,:]$/) && ($thisline !~ /warning/) && ($thisline !~ /note/))
        {
            $thisline =~ s/(\d+:\s+)/$1$col_default$col_error/;
            $thisline = $error_highlight . $thisline . $col_norm;
        }
        else
        {
            # warning
            $thisline =~ s|(warning:\s+)(.*)$|$1$col_warning$2|;
        }

        # In file included from main.cpp:38:
        # main.cpp: In function int main(...)':
        $thisline =~ s/(In f(unction|ile))/$col_trace$1/x;

        # /blah/blah/blah.cpp:123:
        $thisline =~ s|^([^:]+)|$col_filename$1$col_default|;
        $thisline =~ s|:(\d+)([:,])|:$col_linenum$1$col_default$2|;
    }
    elsif ($in eq 'test')
    {
        if ($thisline =~ /passed/)
        {
            $thisline =~ s/(passed)/$col_default$1/x;
            $thisline = $error_highlight . $thisline . $col_norm;
        }

        $in = 'unknown';
    }

    if ($thisline !~ /^\s+/)
    {
        print $col_norm, $col_default;
    }
    print $thisline;
}

print $col_norm;

# UNUSED:
#
#%colors = (
#    'black'     => "\033[30m",
#    'red'       => "\033[31m",
#    'green'     => "\033[32m",
#    'yellow'    => "\033[33m",
#    'blue'      => "\033[34m",
#    'magenta'   => "\033[35m",
#    'purple'    => "\033[35m",
#    'cyan'      => "\033[36m",
#    'white'     => "\033[37m",
#    'darkgray'  => "\033[30m");
