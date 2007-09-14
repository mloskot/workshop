#! /usr/bin/env python
"""Request building GDAL on the remote Buildbot instance.

Usage: gdalbb.py [option]

Options:
    -h           Print this message and exit.
    -l           List builders available to run
    -f           Run all 'full' builders in a batch
    -q           Run all 'quick' builders in a batch
    -b <builder> Run selected builder

Mateusz Loskot <mateusz@loskot.net>
"""
### BEGIN CONFIGURATION BLOCK ###
BBURL = 'http://buildbot.osgeo.org:8500/'
BBUSER = 'mloskot'
BBNOTE = 'Routine build'
#### END CONFIGURATION BLOCK ####
import sys
import getopt
import urllib
import urllib2
import xml.parsers.expat

def find(f, seq):
    """Return first item in sequence where f(item) == True."""
    for item in seq:
        if f(item): 
            return item

class BuildersFinder:

    pxml = None
    found = False
    quick = []
    full = []
    other = []

    def __init__(self):
        self.pxml = xml.parsers.expat.ParserCreate()
        self.pxml.StartElementHandler = self.handleStartElement
        self.pxml.EndElementHandler = self.handleEndElement
        self.pxml.CharacterDataHandler = self.handleCharData
        self.found = False

    def handleStartElement(self, name, attrs):
        if name == 'td':
            a = find(lambda k: k == 'class', attrs.keys())
            if a is not None:
                if attrs[a] == 'Builder':
                    self.found = True

        if self.found is True and name == 'a':
            bldr = attrs['href']
            type = bldr[-4:]
            if type == 'uick':
                self.quick.append(bldr)
            elif type == 'full':
                self.full.append(bldr)
            else:
                self.other.append(bldr)
            self.found = False;
    
    def handleEndElement(self, name):
        pass

    def handleCharData(self, data):
        pass

    def collect(self, html):
        assert(self.pxml is not None)
        self.pxml.Parse(html);

class Usage(Exception):
    def __init__(self, msg):
        self.msg = msg

def usage(code):
    print >> sys.stderr, __doc__
    return code

def list(builders, msg):
    print msg,':'
    i = 0
    for b in builders:
        i = i + 1
        print '%d. %s' % (i, b)

def build(builders, data, msg):
    print msg,':'
    tmp = urllib.urlencode(data)
    url = BBURL + '%s/force'
    i  = 0
    for b in builders:
        i = i + 1
        addr = url % b
        print " %d. %s" % (i, b)
        request = urllib2.Request(addr, tmp)
        response = urllib2.urlopen(request)

def main(argv):

    url = BBURL 
    values = { 'username' : BBUSER, 'comments' : BBNOTE }

    try:
        try:
            opts, args = getopt.getopt(argv, 'hlfqb:', [])
            if len(opts) == 0:
                sys.exit(usage(0))
        except getopt.GetoptError, msg:
            raise Usage(msg)

        for opt, arg in opts:
            if opt == '-h':
                usage(0)
                break
            else:
                print "Connecting to GDAL Buildbot instance: %s" % BBURL
                response = urllib2.urlopen(url)
                html = response.read()
                finder = BuildersFinder()
                finder.collect(html)

                if opt == '-l' or opt == '-b':
                    tmp = finder.quick
                    tmp += finder.full

                    if opt == '-l':
                        list(tmp, 'List of available builders')
                        break
                    
                    for b in tmp:
                        if b == arg:
                            build(tmp, values, 'Building GDAL on')
                            break
                    raise ValueError, 'Requested builder \'%s\' is unavailable' % arg
                elif opt == '-f':
                    build(finder.full, values, 'Building GDAL on')
                    break
                elif opt == '-q':
                    build(finder.quick, values, 'Building GDAL on')
                    break
        return 0

    except urllib2.HTTPError, err:
        print >> sys.stderr, 'Error: HTTP', err.code, '-', err.msg 
    except urllib2.URLError, err:
        print >> sys.stderr, 'Error: ', err.reason
    except Usage, err:
        print >> sys.stderr, 'Error: ', err.msg
        print >> sys.stderr, 'Use -h to get usage options'
    except Exception, err:
        print >> sys.stderr, 'Error: ', err

    return 2

# end of main()

if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))

