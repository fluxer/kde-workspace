#!/usr/bin/env python
# -*- coding: UTF-8 -*-

# Extract the PO template of XScreenSaver's hacks configurations.
# Provide the target PO template path and one or more XScreenSaver
# source distribution packages. For example:
#
#    cd $SOMEWHERE/kscreensaver/kxsconfig
#    ./create-hacks-pot.py hacks.pot $OTHERWHERE/xscreensaver-*.tar.gz
#
# The created hacks.pot file in the current working directory
# should then be committed.
# Try to support last 3 years of XScreenSaver releases.
#
# Chusslove Illich <caslav.ilic@gmx.net>

import sys
import os
import glob
import re
import shutil
import tarfile
import xml.parsers.expat


def main ():

    if len(sys.argv) < 3:
        error("Usage: %s hacks.pot xscreensaver-X.Y.tar.gz..."
              % os.path.basename(sys.argv[0]))
    potpath = sys.argv[1]
    xsdistpaths = sys.argv[2:]

    xcfgex = XSCfgExtractor()
    xcfgex.extract(potpath, xsdistpaths)


def error (msg):

    print "%s [error]: %s" % (os.path.basename(sys.argv[0]), msg)
    sys.exit(1)


class XSCfgExtractor:

    def __init__ (self):

        pass


    def _prepare_xml_parser (self, relpath, releaseid):

        self._parser = xml.parsers.expat.ParserCreate()
        self._parser.UseForeignDTD() # to ignore non-default XML entities

        self._parser.StartElementHandler = self._handler_start_element
        self._parser.EndElementHandler = self._handler_end_element
        self._parser.CharacterDataHandler = self._handler_character_data

        # Data structures used in handlers
        self._element_stack = []
        self._releaseid = releaseid
        self._relpath = relpath
        self._chardata = []

        return self._parser


    def extract (self, potpath, distpaths):

        # Extract messages from all source packages.
        self.msgs = []
        tmpdir = "./xscreensaver-unpacked"
        if os.path.exists(tmpdir):
            shutil.rmtree(tmpdir)
        os.mkdir(tmpdir)

        for distpath in distpaths:
            # Extract release ID from the path.
            try:
                distbase = os.path.basename(distpath)
                p3 = distbase.rfind(".tar")
                p2 = distbase.rfind(".", 0, p3 - 1)
                p1 = distbase.rfind("-", 0, p2 - 1)
                majver = int(distbase[p1 + 1:p2])
                minver = int(distbase[p2 + 1:p3])
            except:
                error("Archive file name '%s' not in the form '%s'."
                      % (distpath, "*-X.Y.tar.*"))
            releaseid = (majver, minver)

            # Unpack the source package and collect paths for extraction.
            tar = tarfile.open(distpath)
            tar.extractall(tmpdir)
            tar.close()
            srcdir = glob.glob(os.path.join(tmpdir, "*"))
            if len(srcdir) != 1:
                error("There should be only one top-level item "
                      "in archive '%s'." % distpath)
            srcdir = srcdir[0]
            if not os.path.isdir(srcdir):
                error("The top-level item in archive '%s' "
                      "is not a directory." % distpath)
            cfgdir = os.path.join(srcdir, "hacks", "config")
            cfgpaths = glob.glob(os.path.join(cfgdir, "*.xml"))
            cfgpaths.sort()

            # Extract messages from collected paths.
            for cfgpath in cfgpaths:

                # Make the path relative to source directory.
                relpath = cfgpath.replace(srcdir, "", 1)
                if relpath.startswith(os.path.sep):
                    relpath = relpath[len(os.path.sep):]

                # Parse file.
                p = self._prepare_xml_parser(relpath, releaseid)
                fh = open(cfgpath, "r")
                p.ParseFile(fh)
                fh.close()

            shutil.rmtree(srcdir)

        shutil.rmtree(tmpdir)

        # Group messages by keys, preserving order of appearance.
        # Add paths only from the latest release where the message appears.
        self.msgs.sort(key=lambda m: [-v for v in m[4]])
        # ...sort descending by release ID, stable.
        msg_keys = []
        msgs_by_key = {}
        newest_releaseid = (0, 0)
        for msgctxt, msgid, path, lno, releaseid in self.msgs:
            msgkey = "%s\x04%s" % (msgctxt, msgid)
            msg = msgs_by_key.get(msgkey)
            if msg is None:
                msg = (msgctxt, msgid, [], releaseid)
                msgs_by_key[msgkey] = msg
                msg_keys.append(msgkey)
            if releaseid == msg[3]:
                msg[2].append((path, lno))
            newest_releaseid = max(newest_releaseid, releaseid)

        # Format messages into lines.
        msglines = []
        for msg in [msgs_by_key[x] for x in msg_keys]:
            msgctxt, msgid, srcrefs, releaseid = msg
            if releaseid != newest_releaseid:
                msglines.append("#. last-release: %d.%d\n" % releaseid)
            if srcrefs:
                srcrefstrs = ["%s:%s" % x for x in srcrefs]
                msglines.append("#: %s\n" % " ".join(srcrefstrs))
            if msgctxt is not None:
                msglines.append("msgctxt \"%s\"\n" % poescape(msgctxt))
            msglines.append("msgid \"%s\"\n" % poescape(msgid))
            msglines.append("msgstr \"\"\n")
            msglines.append("\n")

        # Write POT file.
        fh = open(potpath, "w")
        fh.writelines([x.encode("utf-8") for x in msglines])
        fh.close()


    def _handler_start_element (self, name, attrs):

        self._element_stack.append((name, attrs))
        self._startel_lno = self._parser.CurrentLineNumber

        releaseid = self._releaseid
        path = self._relpath
        lno = self._startel_lno

        if 0: pass

        elif name == "screensaver":
            msgid = attrs.get("_label")
            if msgid:
                ctxt = "@item screen saver name"
                self.msgs.append((ctxt, msgid, path, lno, releaseid))
            self._ssname_lno = lno
            self._ssname_pos = len(self.msgs) - 1

        elif name == "_description":
            self._chardata = []
            # Message created on end element.

        elif name == "number":
            label = attrs.get("_label")
            if label:
                ntype = attrs.get("type")
                ctxt = None
                if ntype == "slider":
                    ctxt = "@label:slider"
                elif ntype == "spinbutton":
                    ctxt = "@label:spinbox"
                self.msgs.append((ctxt, label, path, lno, releaseid))
            for limattr in ("_low-label", "_high-label"):
                limlabel = attrs.get(limattr)
                if limlabel:
                    if label:
                        ctxt = "@item:inrange %s" % label
                    else:
                        ctxt = "@item:inrange"
                    self.msgs.append((ctxt, limlabel, path, lno, releaseid))

        elif name == "boolean":
            label = attrs.get("_label")
            if label:
                ctxt = "@option:check"
                self.msgs.append((ctxt, label, path, lno, releaseid))

        elif name == "string":
            label = attrs.get("_label")
            if label:
                ctxt = "@label:textbox"
                self.msgs.append((ctxt, label, path, lno, releaseid))

        elif name == "file":
            label = attrs.get("_label")
            if label:
                ctxt = "@label:chooser"
                self.msgs.append((ctxt, label, path, lno, releaseid))

        elif name == "select":
            label = attrs.get("_label")
            if label:
                ctxt = "@title:group"
                self.msgs.append((ctxt, label, path, lno, releaseid))

        elif name == "option":
            label = attrs.get("_label")
            if label:
                prevlabel = self._element_stack[-2][1].get("_label")
                if prevlabel:
                    ctxt = "@option:radio %s" % prevlabel
                else:
                    ctxt = "@option:radio"
                self.msgs.append((ctxt, label, path, lno, releaseid))


        #print name, attrs, self.parser.CurrentLineNumber


    def _handler_end_element (self, name):

        name, attr = self._element_stack.pop()
        releaseid = self._releaseid
        path = self._relpath
        lno = self._startel_lno

        if name == "_description":
            text = "".join(self._chardata)
            rx = re.compile(r"\s+", re.U)
            text = rx.sub(" ", text.strip())
            ctxt = "@info screen saver description"
            if text:
                # Fudge position of the message,
                # to have description right after the name in the PO file.
                msg = (ctxt, text, path, self._ssname_lno, releaseid)
                self.msgs.insert(self._ssname_pos + 1, msg)


    def _handler_character_data (self, text):

        self._chardata.append(text)


def poescape (text):

    text = text.replace("\"", "\\\"")
    text = text.replace("\n", "\\n")
    text = text.replace("\t", "\\t")

    return text


if __name__ == "__main__":
    main()
