/* $XFree86: xc/programs/xterm/wcwidth.characters,v 1.9 2006/06/19 00:36:52 dickey Exp $ */

/*
 * This is an implementation of wcwidth() and wcswidth() (defined in
 * IEEE Std 1002.1-2001) for Unicode.
 *
 * http://www.opengroup.org/onlinepubs/007904975/functions/wcwidth.html
 * http://www.opengroup.org/onlinepubs/007904975/functions/wcswidth.html
 *
 * In fixed-width output devices, Latin characters all occupy a single
 * "cell" position of equal width, whereas ideographic CJK characters
 * occupy two such cells. Interoperability between terminal-line
 * applications and (teletype-style) character terminals using the
 * UTF-8 encoding requires agreement on which character should advance
 * the cursor by how many cell positions. No established formal
 * standards exist at present on which Unicode character shall occupy
 * how many cell positions on character terminals. These routines are
 * a first attempt of defining such behavior based on simple rules
 * applied to data provided by the Unicode Consortium.
 *
 * For some graphical characters, the Unicode standard explicitly
 * defines a character-cell width via the definition of the East Asian
 * FullWidth (F), Wide (W), Half-width (H), and Narrow (Na) classes.
 * In all these cases, there is no ambiguity about which width a
 * terminal shall use. For characters in the East Asian Ambiguous (A)
 * class, the width choice depends purely on a preference of backward
 * compatibility with either historic CJK or Western practice.
 * Choosing single-width for these characters is easy to justify as
 * the appropriate long-term solution, as the CJK practice of
 * displaying these characters as double-width comes from historic
 * implementation simplicity (8-bit encoded characters were displayed
 * single-width and 16-bit ones double-width, even for Greek,
 * Cyrillic, etc.) and not any typographic considerations.
 *
 * Much less clear is the choice of width for the Not East Asian
 * (Neutral) class. Existing practice does not dictate a width for any
 * of these characters. It would nevertheless make sense
 * typographically to allocate two character cells to characters such
 * as for instance EM SPACE or VOLUME INTEGRAL, which cannot be
 * represented adequately with a single-width glyph. The following
 * routines at present merely assign a single-cell width to all
 * neutral characters, in the interest of simplicity. This is not
 * entirely satisfactory and should be reconsidered before
 * establishing a formal standard in this area. At the moment, the
 * decision which Not East Asian (Neutral) characters should be
 * represented by double-width glyphs cannot yet be answered by
 * applying a simple rule from the Unicode database content. Setting
 * up a proper standard for the behavior of UTF-8 character terminals
 * will require a careful analysis not only of each Unicode character,
 * but also of each presentation form, something the author of these
 * routines has avoided to do so far.
 *
 * http://www.unicode.org/unicode/reports/tr11/
 *
 * Markus Kuhn -- 2007-05-25 (Unicode 5.0)
 *
 * Permission to use, copy, modify, and distribute this software
 * for any purpose and without fee is hereby granted. The author
 * disclaims all warranties with regard to this software.
 *
 * Latest version: http://www.cl.cam.ac.uk/~mgk25/ucs/wcwidth.c
 */
/*
 *  Adaptions for KDE by Waldo Bastian <bastian@kde.org> and
 *    Francesco Cecconi <francesco.cecconi@gmail.com>
 *  See COPYING.Unicode for the license for the original wcwidth.c
 */

// Own
#include "konsole_wcwidth.h"
#include "konsoleprivate_export.h"

struct interval {
    unsigned long first;
    unsigned long last;
};

/* auxiliary function for binary search in interval table */
static int bisearch(unsigned long ucs, const struct interval* table, int max)
{
    int min = 0;
    int mid;

    if (ucs < table[0].first || ucs > table[max].last)
        return 0;
    while (max >= min) {
        mid = (min + max) / 2;
        if (ucs > table[mid].last)
            min = mid + 1;
        else if (ucs < table[mid].first)
            max = mid - 1;
        else
            return 1;
    }

    return 0;
}

/* The following functions define the column width of an ISO 10646
 * character as follows:
 *
 *    - The null character (U+0000) has a column width of 0.
 *
 *    - Other C0/C1 control characters and DEL will lead to a return
 *      value of -1.
 *
 *    - Non-spacing and enclosing combining characters (general
 *      category code Mn or Me in the Unicode database) have a
 *      column width of 0.
 *
 *    - Other format characters (general category code Cf in the Unicode
 *      database) and ZERO WIDTH SPACE (U+200B) have a column width of 0.
 *
 *    - Hangul Jamo medial vowels and final consonants (U+1160-U+11FF)
 *      have a column width of 0.
 *
 *    - Spacing characters in the East Asian Wide (W) or East Asian
 *      FullWidth (F) category as defined in Unicode Technical
 *      Report #11 have a column width of 2.
 *
 *    - All remaining characters (including all printable
 *      ISO 8859-1 and WGL4 characters, Unicode control characters,
 *      etc.) have a column width of 1.
 *
 * This implementation assumes that quint16 characters are encoded
 * in ISO 10646.
 */

int KONSOLEPRIVATE_EXPORT konsole_wcwidth(quint16 oucs)
{
    /* NOTE: uniset script can be obtained from:
     * https://fossies.org/linux/R/tools/uniset
     */
    /* NOTE: It is not possible to compare quint16 with the new last four lines of characters,
     * therefore this cast is now necessary.
     */
    unsigned long ucs = static_cast<unsigned long>(oucs);
    /* sorted list of non-overlapping intervals of non-spacing characters */
    /* generated by "uniset +cat=Me +cat=Mn +cat=Cf -00AD +1160-11FF +200B c" */
    static const struct interval combining[] = {
        { 0x300, 0x36f },
        { 0x483, 0x489 },
        { 0x591, 0x5bd },
        { 0x5bf, 0x5bf },
        { 0x5c1, 0x5c2 },
        { 0x5c4, 0x5c5 },
        { 0x5c7, 0x5c7 },
        { 0x600, 0x605 },
        { 0x610, 0x61a },
        { 0x61c, 0x61c },
        { 0x64b, 0x65f },
        { 0x670, 0x670 },
        { 0x6d6, 0x6dd },
        { 0x6df, 0x6e4 },
        { 0x6e7, 0x6e8 },
        { 0x6ea, 0x6ed },
        { 0x70f, 0x70f },
        { 0x711, 0x711 },
        { 0x730, 0x74a },
        { 0x7a6, 0x7b0 },
        { 0x7eb, 0x7f3 },
        { 0x7fd, 0x7fd },
        { 0x816, 0x819 },
        { 0x81b, 0x823 },
        { 0x825, 0x827 },
        { 0x829, 0x82d },
        { 0x859, 0x85b },
        { 0x8d3, 0x902 },
        { 0x93a, 0x93a },
        { 0x93c, 0x93c },
        { 0x941, 0x948 },
        { 0x94d, 0x94d },
        { 0x951, 0x957 },
        { 0x962, 0x963 },
        { 0x981, 0x981 },
        { 0x9bc, 0x9bc },
        { 0x9c1, 0x9c4 },
        { 0x9cd, 0x9cd },
        { 0x9e2, 0x9e3 },
        { 0x9fe, 0x9fe },
        { 0xa01, 0xa02 },
        { 0xa3c, 0xa3c },
        { 0xa41, 0xa42 },
        { 0xa47, 0xa48 },
        { 0xa4b, 0xa4d },
        { 0xa51, 0xa51 },
        { 0xa70, 0xa71 },
        { 0xa75, 0xa75 },
        { 0xa81, 0xa82 },
        { 0xabc, 0xabc },
        { 0xac1, 0xac5 },
        { 0xac7, 0xac8 },
        { 0xacd, 0xacd },
        { 0xae2, 0xae3 },
        { 0xafa, 0xaff },
        { 0xb01, 0xb01 },
        { 0xb3c, 0xb3c },
        { 0xb3f, 0xb3f },
        { 0xb41, 0xb44 },
        { 0xb4d, 0xb4d },
        { 0xb55, 0xb56 },
        { 0xb62, 0xb63 },
        { 0xb82, 0xb82 },
        { 0xbc0, 0xbc0 },
        { 0xbcd, 0xbcd },
        { 0xc00, 0xc00 },
        { 0xc04, 0xc04 },
        { 0xc3e, 0xc40 },
        { 0xc46, 0xc48 },
        { 0xc4a, 0xc4d },
        { 0xc55, 0xc56 },
        { 0xc62, 0xc63 },
        { 0xc81, 0xc81 },
        { 0xcbc, 0xcbc },
        { 0xcbf, 0xcbf },
        { 0xcc6, 0xcc6 },
        { 0xccc, 0xccd },
        { 0xce2, 0xce3 },
        { 0xd00, 0xd01 },
        { 0xd3b, 0xd3c },
        { 0xd41, 0xd44 },
        { 0xd4d, 0xd4d },
        { 0xd62, 0xd63 },
        { 0xd81, 0xd81 },
        { 0xdca, 0xdca },
        { 0xdd2, 0xdd4 },
        { 0xdd6, 0xdd6 },
        { 0xe31, 0xe31 },
        { 0xe34, 0xe3a },
        { 0xe47, 0xe4e },
        { 0xeb1, 0xeb1 },
        { 0xeb4, 0xebc },
        { 0xec8, 0xecd },
        { 0xf18, 0xf19 },
        { 0xf35, 0xf35 },
        { 0xf37, 0xf37 },
        { 0xf39, 0xf39 },
        { 0xf71, 0xf7e },
        { 0xf80, 0xf84 },
        { 0xf86, 0xf87 },
        { 0xf8d, 0xf97 },
        { 0xf99, 0xfbc },
        { 0xfc6, 0xfc6 },
        { 0x102d, 0x1030 },
        { 0x1032, 0x1037 },
        { 0x1039, 0x103a },
        { 0x103d, 0x103e },
        { 0x1058, 0x1059 },
        { 0x105e, 0x1060 },
        { 0x1071, 0x1074 },
        { 0x1082, 0x1082 },
        { 0x1085, 0x1086 },
        { 0x108d, 0x108d },
        { 0x109d, 0x109d },
        { 0x1160, 0x11ff },
        { 0x135d, 0x135f },
        { 0x1712, 0x1714 },
        { 0x1732, 0x1734 },
        { 0x1752, 0x1753 },
        { 0x1772, 0x1773 },
        { 0x17b4, 0x17b5 },
        { 0x17b7, 0x17bd },
        { 0x17c6, 0x17c6 },
        { 0x17c9, 0x17d3 },
        { 0x17dd, 0x17dd },
        { 0x180b, 0x180e },
        { 0x1885, 0x1886 },
        { 0x18a9, 0x18a9 },
        { 0x1920, 0x1922 },
        { 0x1927, 0x1928 },
        { 0x1932, 0x1932 },
        { 0x1939, 0x193b },
        { 0x1a17, 0x1a18 },
        { 0x1a1b, 0x1a1b },
        { 0x1a56, 0x1a56 },
        { 0x1a58, 0x1a5e },
        { 0x1a60, 0x1a60 },
        { 0x1a62, 0x1a62 },
        { 0x1a65, 0x1a6c },
        { 0x1a73, 0x1a7c },
        { 0x1a7f, 0x1a7f },
        { 0x1ab0, 0x1ac0 },
        { 0x1b00, 0x1b03 },
        { 0x1b34, 0x1b34 },
        { 0x1b36, 0x1b3a },
        { 0x1b3c, 0x1b3c },
        { 0x1b42, 0x1b42 },
        { 0x1b6b, 0x1b73 },
        { 0x1b80, 0x1b81 },
        { 0x1ba2, 0x1ba5 },
        { 0x1ba8, 0x1ba9 },
        { 0x1bab, 0x1bad },
        { 0x1be6, 0x1be6 },
        { 0x1be8, 0x1be9 },
        { 0x1bed, 0x1bed },
        { 0x1bef, 0x1bf1 },
        { 0x1c2c, 0x1c33 },
        { 0x1c36, 0x1c37 },
        { 0x1cd0, 0x1cd2 },
        { 0x1cd4, 0x1ce0 },
        { 0x1ce2, 0x1ce8 },
        { 0x1ced, 0x1ced },
        { 0x1cf4, 0x1cf4 },
        { 0x1cf8, 0x1cf9 },
        { 0x1dc0, 0x1df9 },
        { 0x1dfb, 0x1dff },
        { 0x200b, 0x200f },
        { 0x202a, 0x202e },
        { 0x2060, 0x2064 },
        { 0x2066, 0x206f },
        { 0x20d0, 0x20f0 },
        { 0x2cef, 0x2cf1 },
        { 0x2d7f, 0x2d7f },
        { 0x2de0, 0x2dff },
        { 0x302a, 0x302d },
        { 0x3099, 0x309a },
        { 0xa66f, 0xa672 },
        { 0xa674, 0xa67d },
        { 0xa69e, 0xa69f },
        { 0xa6f0, 0xa6f1 },
        { 0xa802, 0xa802 },
        { 0xa806, 0xa806 },
        { 0xa80b, 0xa80b },
        { 0xa825, 0xa826 },
        { 0xa82c, 0xa82c },
        { 0xa8c4, 0xa8c5 },
        { 0xa8e0, 0xa8f1 },
        { 0xa8ff, 0xa8ff },
        { 0xa926, 0xa92d },
        { 0xa947, 0xa951 },
        { 0xa980, 0xa982 },
        { 0xa9b3, 0xa9b3 },
        { 0xa9b6, 0xa9b9 },
        { 0xa9bc, 0xa9bd },
        { 0xa9e5, 0xa9e5 },
        { 0xaa29, 0xaa2e },
        { 0xaa31, 0xaa32 },
        { 0xaa35, 0xaa36 },
        { 0xaa43, 0xaa43 },
        { 0xaa4c, 0xaa4c },
        { 0xaa7c, 0xaa7c },
        { 0xaab0, 0xaab0 },
        { 0xaab2, 0xaab4 },
        { 0xaab7, 0xaab8 },
        { 0xaabe, 0xaabf },
        { 0xaac1, 0xaac1 },
        { 0xaaec, 0xaaed },
        { 0xaaf6, 0xaaf6 },
        { 0xabe5, 0xabe5 },
        { 0xabe8, 0xabe8 },
        { 0xabed, 0xabed },
        { 0xfb1e, 0xfb1e },
        { 0xfe00, 0xfe0f },
        { 0xfe20, 0xfe2f },
        { 0xfeff, 0xfeff },
        { 0xfff9, 0xfffb },
        { 0x101fd, 0x101fd },
        { 0x102e0, 0x102e0 },
        { 0x10376, 0x1037a },
        { 0x10a01, 0x10a03 },
        { 0x10a05, 0x10a06 },
        { 0x10a0c, 0x10a0f },
        { 0x10a38, 0x10a3a },
        { 0x10a3f, 0x10a3f },
        { 0x10ae5, 0x10ae6 },
        { 0x10d24, 0x10d27 },
        { 0x10eab, 0x10eac },
        { 0x10f46, 0x10f50 },
        { 0x11001, 0x11001 },
        { 0x11038, 0x11046 },
        { 0x1107f, 0x11081 },
        { 0x110b3, 0x110b6 },
        { 0x110b9, 0x110ba },
        { 0x110bd, 0x110bd },
        { 0x110cd, 0x110cd },
        { 0x11100, 0x11102 },
        { 0x11127, 0x1112b },
        { 0x1112d, 0x11134 },
        { 0x11173, 0x11173 },
        { 0x11180, 0x11181 },
        { 0x111b6, 0x111be },
        { 0x111c9, 0x111cc },
        { 0x111cf, 0x111cf },
        { 0x1122f, 0x11231 },
        { 0x11234, 0x11234 },
        { 0x11236, 0x11237 },
        { 0x1123e, 0x1123e },
        { 0x112df, 0x112df },
        { 0x112e3, 0x112ea },
        { 0x11300, 0x11301 },
        { 0x1133b, 0x1133c },
        { 0x11340, 0x11340 },
        { 0x11366, 0x1136c },
        { 0x11370, 0x11374 },
        { 0x11438, 0x1143f },
        { 0x11442, 0x11444 },
        { 0x11446, 0x11446 },
        { 0x1145e, 0x1145e },
        { 0x114b3, 0x114b8 },
        { 0x114ba, 0x114ba },
        { 0x114bf, 0x114c0 },
        { 0x114c2, 0x114c3 },
        { 0x115b2, 0x115b5 },
        { 0x115bc, 0x115bd },
        { 0x115bf, 0x115c0 },
        { 0x115dc, 0x115dd },
        { 0x11633, 0x1163a },
        { 0x1163d, 0x1163d },
        { 0x1163f, 0x11640 },
        { 0x116ab, 0x116ab },
        { 0x116ad, 0x116ad },
        { 0x116b0, 0x116b5 },
        { 0x116b7, 0x116b7 },
        { 0x1171d, 0x1171f },
        { 0x11722, 0x11725 },
        { 0x11727, 0x1172b },
        { 0x1182f, 0x11837 },
        { 0x11839, 0x1183a },
        { 0x1193b, 0x1193c },
        { 0x1193e, 0x1193e },
        { 0x11943, 0x11943 },
        { 0x119d4, 0x119d7 },
        { 0x119da, 0x119db },
        { 0x119e0, 0x119e0 },
        { 0x11a01, 0x11a0a },
        { 0x11a33, 0x11a38 },
        { 0x11a3b, 0x11a3e },
        { 0x11a47, 0x11a47 },
        { 0x11a51, 0x11a56 },
        { 0x11a59, 0x11a5b },
        { 0x11a8a, 0x11a96 },
        { 0x11a98, 0x11a99 },
        { 0x11c30, 0x11c36 },
        { 0x11c38, 0x11c3d },
        { 0x11c3f, 0x11c3f },
        { 0x11c92, 0x11ca7 },
        { 0x11caa, 0x11cb0 },
        { 0x11cb2, 0x11cb3 },
        { 0x11cb5, 0x11cb6 },
        { 0x11d31, 0x11d36 },
        { 0x11d3a, 0x11d3a },
        { 0x11d3c, 0x11d3d },
        { 0x11d3f, 0x11d45 },
        { 0x11d47, 0x11d47 },
        { 0x11d90, 0x11d91 },
        { 0x11d95, 0x11d95 },
        { 0x11d97, 0x11d97 },
        { 0x11ef3, 0x11ef4 },
        { 0x13430, 0x13438 },
        { 0x16af0, 0x16af4 },
        { 0x16b30, 0x16b36 },
        { 0x16f4f, 0x16f4f },
        { 0x16f8f, 0x16f92 },
        { 0x16fe4, 0x16fe4 },
        { 0x1bc9d, 0x1bc9e },
        { 0x1bca0, 0x1bca3 },
        { 0x1d167, 0x1d169 },
        { 0x1d173, 0x1d182 },
        { 0x1d185, 0x1d18b },
        { 0x1d1aa, 0x1d1ad },
        { 0x1d242, 0x1d244 },
        { 0x1da00, 0x1da36 },
        { 0x1da3b, 0x1da6c },
        { 0x1da75, 0x1da75 },
        { 0x1da84, 0x1da84 },
        { 0x1da9b, 0x1da9f },
        { 0x1daa1, 0x1daaf },
        { 0x1e000, 0x1e006 },
        { 0x1e008, 0x1e018 },
        { 0x1e01b, 0x1e021 },
        { 0x1e023, 0x1e024 },
        { 0x1e026, 0x1e02a },
        { 0x1e130, 0x1e136 },
        { 0x1e2ec, 0x1e2ef },
        { 0x1e8d0, 0x1e8d6 },
        { 0x1e944, 0x1e94a },
        { 0xe0001, 0xe0001 },
        { 0xe0020, 0xe007f },
        { 0xe0100, 0xe01ef }
    };

    /* test for 8-bit control characters */
    if (ucs == 0)
        return 0;
    if (ucs < 32 || (ucs >= 0x7f && ucs < 0xa0))
        return -1;

    /* binary search in table of non-spacing characters */
    if (bisearch(ucs, combining,
                 sizeof(combining) / sizeof(struct interval) - 1))
        return 0;

    /* if we arrive here, ucs is not a combining or C0/C1 control character */

    return 1 +
           (ucs >= 0x1100 &&
            (ucs <= 0x115f ||                    /* Hangul Jamo init. consonants */
             ucs == 0x2329 || ucs == 0x232a ||
             (ucs >= 0x2e80 && ucs <= 0xa4cf &&
              ucs != 0x303f) ||                  /* CJK ... Yi */
             (ucs >= 0xac00 && ucs <= 0xd7a3) || /* Hangul Syllables */
             (ucs >= 0xf900 && ucs <= 0xfaff) || /* CJK Compatibility Ideographs */
             (ucs >= 0xfe10 && ucs <= 0xfe19) || /* Vertical forms */
             (ucs >= 0xfe30 && ucs <= 0xfe6f) || /* CJK Compatibility Forms */
             (ucs >= 0xff00 && ucs <= 0xff60) || /* Fullwidth Forms */
             (ucs >= 0xffe0 && ucs <= 0xffe6) ||
             (ucs >= 0x20000 && ucs <= 0x2fffd) ||
             (ucs >= 0x30000 && ucs <= 0x3fffd)));
}

int string_width(const QString& text)
{
    int w = 0;
    for (int i = 0; i < text.length(); ++i)
        w += konsole_wcwidth(text[i].unicode());
    return w;
}

