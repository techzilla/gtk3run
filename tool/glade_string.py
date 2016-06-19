#!/usr/bin/env python3
"""@package docstring
Generate C Header from Glade UI
"""
import argparse
import os
import xml.dom.minidom


class XmlWriter:
    def __init__(self):
        self.snippets = []
    def write(self, data):
        if data.isspace(): return
        self.snippets.append(data)
    def __str__(self):
        return ''.join(self.snippets)


class GladeString:
    def __init__(self):
        '''
        Main execution path
        '''

        self.ParseCliArgs()
        
        self.CreateHeader()

    def ParseCliArgs(self):
        '''
        Command line argument processing
        '''

        parser = argparse.ArgumentParser(description='Create C Header from Glade UI')
        parser.add_argument('-f', action='store', dest='filename',
                           help='Input Filename')
        self.args = parser.parse_args()


    def CreateHeader(self):
        writer = XmlWriter()
        self.xml = xml.dom.minidom.parse(self.args.filename)
        self.xml.writexml(writer)
        strippedXml = ("%s" % (writer)).replace('"', '\\"')
        byteCount = len(strippedXml)

        baseOffset=0
        stripSize=96

        output = open((os.path.splitext(self.args.filename)[0]+'.h'), 'w')
        output.write("static const char gladestring [] = \n{\n")
        while baseOffset < byteCount:
            skipTrailingQuote = 0
            if baseOffset+stripSize < byteCount and strippedXml[baseOffset+stripSize] == '"':
                skipTrailingQuote = 1
            output.write('  "%s"\n' % (strippedXml[baseOffset:baseOffset+stripSize+skipTrailingQuote]))
            baseOffset += stripSize + skipTrailingQuote
        
        output.write("};\n")
        output.close()


def main():
    GladeString()

if __name__ == '__main__':
    main()

