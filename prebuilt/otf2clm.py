#!/usr/bin/python3
# -*- coding: utf-8 -*-

# Parse open-type font file and convert to `clm` format file

import sys
from functools import reduce, partial
import fontforge
import xml.dom.minidom as dom
import tempfile
import struct


def chain(*fs):
    def _chain(f, g):
        return lambda x: g(f(x))

    return reduce(_chain, fs, lambda x: x)


def fmap(f, it):
    return [j for i in it for j in f(i)]


def do(f, x):
    f(x)
    return x


def do_loop(f, xs):
    for x in xs:
        f(x)
    return xs


def find_first(xs, predicate):
    for x in xs:
        if predicate(x):
            return x
    return None


def read_math_consts(font):
    m = font.math
    return [
        m.ScriptPercentScaleDown,
        m.ScriptScriptPercentScaleDown,
        m.DelimitedSubFormulaMinHeight,
        m.DisplayOperatorMinHeight,
        m.MathLeading,
        m.AxisHeight,
        m.AccentBaseHeight,
        m.FlattenedAccentBaseHeight,
        m.SubscriptShiftDown,
        m.SubscriptTopMax,
        m.SubscriptBaselineDropMin,
        m.SuperscriptShiftUp,
        m.SuperscriptShiftUpCramped,
        m.SuperscriptBottomMin,
        m.SuperscriptBaselineDropMax,
        m.SubSuperscriptGapMin,
        m.SuperscriptBottomMaxWithSubscript,
        m.SpaceAfterScript,
        m.UpperLimitGapMin,
        m.UpperLimitBaselineRiseMin,
        m.LowerLimitGapMin,
        m.LowerLimitBaselineDropMin,
        m.StackTopShiftUp,
        m.StackTopDisplayStyleShiftUp,
        m.StackBottomShiftDown,
        m.StackBottomDisplayStyleShiftDown,
        m.StackGapMin,
        m.StackDisplayStyleGapMin,
        m.StretchStackTopShiftUp,
        m.StretchStackBottomShiftDown,
        m.StretchStackGapAboveMin,
        m.StretchStackGapBelowMin,
        m.FractionNumeratorShiftUp,
        m.FractionNumeratorDisplayStyleShiftUp,
        m.FractionDenominatorShiftDown,
        m.FractionDenominatorDisplayStyleShiftDown,
        m.FractionNumeratorGapMin,
        m.FractionNumeratorDisplayStyleGapMin,
        m.FractionRuleThickness,
        m.FractionDenominatorGapMin,
        m.FractionDenominatorDisplayStyleGapMin,
        m.SkewedFractionHorizontalGap,
        m.SkewedFractionVerticalGap,
        m.OverbarVerticalGap,
        m.OverbarRuleThickness,
        m.OverbarExtraAscender,
        m.UnderbarVerticalGap,
        m.UnderbarRuleThickness,
        m.UnderbarExtraDescender,
        m.RadicalVerticalGap,
        m.RadicalDisplayStyleVerticalGap,
        m.RadicalRuleThickness,
        m.RadicalExtraAscender,
        m.RadicalKernBeforeDegree,
        m.RadicalKernAfterDegree,
        m.RadicalDegreeBottomRaisePercent,
        m.MinConnectorOverlap
    ]


def read_metrics(glyph):
    '''
    Return a tuple to represents metrics, in (width, height, depth) order
    '''
    # [xmin, ymin, xmax, ymax]
    # 0 is the baseline
    bounding_box = glyph.boundingBox()
    return (
        # width
        glyph.width,
        # height = max(0, ymax)
        max(0, bounding_box[3]),
        # depth = -min(0, ymin)
        -min(0, bounding_box[1])
    )


def read_variants(get_variants):
    '''
    Return an array of glyph names to represents variants
    '''
    variants = get_variants()
    if not variants:
        return []
    return variants.split(' ')


def read_scripts(glyph, subtable_names):
    '''
    Return an array of glyph names to represents scripts variants 
    '''
    scripts = chain(
        partial(fmap, lambda subtable_name: glyph.getPosSub(subtable_name)),
        partial(fmap, lambda infos: infos[2:])
    )(subtable_names)
    return list(scripts)


def read_glyph_assembly(get_assembly):
    '''
    Return a tuple
    (
        italics_correction,
        (
            (glyph_name, flag, start_connector_length, end_connector_length, full_advance),
            ...
        )
    )
    '''
    return get_assembly()


def read_math_kern_record(glyph):
    '''
    Return a tuple contains 4 elements in clockwise direction, None means not represent
    (
        ((correction_height, kerning), ...),
        None,
        ...
    )
    '''
    return (
        glyph.mathKern.topLeft,
        glyph.mathKern.topRight,
        glyph.mathKern.bottomLeft,
        glyph.mathKern.bottomRight,
    )


def read_math(glyph, scripts_subtable_names):
    return (
        glyph.italicCorrection,
        # if no top accent attachment, the value == 32767 (undefined math value)
        glyph.topaccent,
        read_variants(lambda: glyph.horizontalVariants),
        read_variants(lambda: glyph.verticalVariants),
        read_scripts(glyph, scripts_subtable_names),
        read_glyph_assembly(lambda: (
            glyph.horizontalComponentItalicCorrection,
            glyph.horizontalComponents,
        )),
        read_glyph_assembly(lambda: (
            glyph.verticalComponentItalicCorrection,
            glyph.verticalComponents,
        )),
        read_math_kern_record(glyph),
    )


def read_kern(glyph, kern_subtable_names):
    '''
    Return array of tuples represents kerning
    [
        (glyph_name, kerning),
        ...
    ]
    '''
    ks = []
    for sn in kern_subtable_names:
        kerns = glyph.getPosSub(sn)
        for k in kerns:
            if k[5] != 0:
                ks.append((k[2], k[5],))
    return ks


def path_cmd_arg_cnt(x):
    if x == 'M' or x == 'm' or x == 'L' or x == 'l' or x == 'T' or x == 't':
        return 2
    if x == 'H' or x == 'h' or x == 'V' or x == 'v':
        return 1
    if x == 'C' or x == 'c':
        return 6
    if x == 'S' or x == 's' or x == 'Q' or x == 'q':
        return 4
    if x == 'Z' or x == 'z':
        return -1
    return 0


def is_valid_digit(c):
    return c == '.' or c == '-' or c.isdigit()


def mirror_cmd(t):
    x = t[0]
    if x == 'M' or x == 'm' or x == 'L' or x == 'l' or x == 'T' or x == 't':
        t[2] = -t[2]
    elif x == 'V' or x == 'v':
        t[1] = -t[1]
    elif x == 'C' or x == 'c':
        t[2] = -t[2]
        t[4] = -t[4]
        t[6] = -t[6]
    elif x == 'S' or x == 's' or x == 'Q' or x == 'q':
        t[2] = -t[2]
        t[4] = -t[4]


def mirror(data):
    for t in data:
        mirror_cmd(t)


def read_glyph_path(glyph):
    tf = tempfile.NamedTemporaryFile(suffix='.svg', mode='w+')
    # keep font coordinate-system, thus increase y from bottom to up
    glyph.export(tf.name, usetransform=True)
    string = tf.read()
    tf.close()

    svg = dom.parseString(string).documentElement
    g = svg.getElementsByTagName('g')
    paths = g[0].getElementsByTagName('path')

    path = None
    if paths:
        path = paths[0]
    if not path:
        return []

    v = path.attributes.get('d').value
    data = []

    def digits(i):
        j = i
        while is_valid_digit(v[j]):
            j += 1
        return (v[i:j], j,)

    i = 0
    while i < len(v):
        x = v[i]
        cnt = path_cmd_arg_cnt(x)
        if not cnt:
            i += 1
            continue
        i += 1
        cmd = [x]
        for j in range(0, cnt):
            while not is_valid_digit(v[i]):
                i += 1
            r = digits(i)
            cmd.append(int(r[0]))
            i = r[1]
        data.append(cmd)

    mirror(data)
    return data


def read_glyph(glyph, is_math_font, parse_path, kern_subtable_names, scripts_subtable_names):
    '''
    Return a tuple
    (
        metrics,
        kerning,
        math,
        path_cmds,
        glyph_name
    )
    '''
    return (
        read_metrics(glyph),
        read_kern(glyph, kern_subtable_names),
        None if not is_math_font else read_math(glyph, scripts_subtable_names),
        None if not parse_path else read_glyph_path(glyph),
        glyph.glyphname,
    )


def _read_lookup_subtables(
        font, is_target_lookup=lambda x: True, is_target_subtable=lambda x: True):
    '''
    Return a function to get the subtable names
    '''

    def _is_target_lookup(lookup_name):
        lookup_info = font.getLookupInfo(lookup_name)
        return lookup_info and is_target_lookup(lookup_info)

    return chain(
        partial(filter, _is_target_lookup),
        partial(fmap, lambda lookup_name: font.getLookupSubtables(lookup_name)),
        partial(filter, is_target_subtable)
    )


def read_scripts_subtables(font):
    '''
    Return array of scripts variants subtable name
    The feature tag is 'ssty'
    '''
    subtables = _read_lookup_subtables(
        font,
        lambda info: info[0] == 'gsub_alternate' and any(
            t[0] == 'ssty' for t in info[2])
    )(font.gsub_lookups)
    return list(subtables)


def read_kern_subtables(font):
    '''
    Return array of kerning subtable name
    '''
    tables = []
    lookups = font.gpos_lookups
    for lookup_name in lookups:
        lookup_info = font.getLookupInfo(lookup_name)
        if lookup_info and lookup_info[0] == 'gpos_pair':
            subtable_names = font.getLookupSubtables(lookup_name)
            for sn in subtable_names:
                if not font.isKerningClass(sn):
                    tables.append(sn)
    return tables


def read_ligature_subtables(font):
    '''
    Return array of ligture subtable name
    '''
    tables = []
    lookups = font.gsub_lookups
    for lookup_name in lookups:
        lookup_info = font.getLookupInfo(lookup_name)
        if lookup_info and lookup_info[0] == 'gsub_ligature':
            subtable_names = font.getLookupSubtables(lookup_name)
            for sn in subtable_names:
                tables.append(sn)
    return tables


def read_ligatures(glyph, liga_subtable_names):
    '''
    Return array of ligatures info represents by this glyph,
    the glyph id is the ligature
    [
        ((glyph_name, glyph_name, ...), glyph_id),
        ...
    ]
    '''
    ligs = []
    for sn in liga_subtable_names:
        liga_infos = glyph.getPosSub(sn)
        for info in liga_infos:
            ligs.append((info[2:], glyph.originalgid,))
    return ligs


def read_kerning_class(font):
    '''
    Return array of kerning class table:
    (
        # glyphs on left
        (
            None, # first is None
            (glyph_name, glyph_name, ...),
            ...
        ),
        # glyphs on right
        (
            None, # first is None
            (glyph_name, glyph_name, ...),
            ...
        ),
        # kerning value
        (
            kerning_value,
            ...
        )
    )
    '''
    kerns = []
    lookups = font.gpos_lookups
    for ln in lookups:
        li = font.getLookupInfo(ln)
        if li and li[0] == 'gpos_pair':
            subtables = font.getLookupSubtables(ln)
            for sn in subtables:
                if font.isKerningClass(sn):
                    kerns.append(font.getKerningClass(sn))
    return kerns


def write_clm_unicode_glyph_map(f, unicode_glyph_map):
    length = len(unicode_glyph_map)
    f.write(struct.pack('!H', length))
    sort_map = sorted(unicode_glyph_map, key=lambda x: x[0])
    for (codepoint, glyph_id,) in sort_map:
        f.write(struct.pack('!IH', codepoint, glyph_id))


def write_clm_kerning_class(f, kerning_classes, glyph_name_id_map):
    length = len(list(kerning_classes))
    f.write(struct.pack('!H', length))

    def write_group(g):
        """
        row format:
        (index, (glyph_name, glyph_name, ...))
        convert to:
        [{index, glyph_id}, ...]
        """
        # write the group length
        f.write(struct.pack('!H', len(g)))
        m = []
        for (index, glyph_names,) in g:
            for name in glyph_names:
                m.append({'index': index, 'glyph_id': glyph_name_id_map[name]})
        # sort the array by glyph id
        m = sorted(m, key=lambda item: item['glyph_id'])
        # write the count of the glyphs
        f.write(struct.pack('!H', len(m)))
        # write glyph id and its index in this group
        for item in m:
            f.write(struct.pack('!HH', item['glyph_id'], item['index']))

    for (left, right, value,) in kerning_classes:
        write_group(list(enumerate(left[1:])))
        write_group(list(enumerate(right[1:])))
        column_count = len(right)
        for i, v in enumerate(value):
            if i < column_count or i % column_count == 0:
                continue
            f.write(struct.pack('!h', v))


def write_ligas(f, ligas, glyph_name_id_map):
    forest = []

    def add_node(glyphs, liga):
        children = forest
        child = None
        for (glyph, char,) in glyphs:
            child = find_first(children, lambda x: x['glyph'] == glyph)
            if not child:
                child = {'glyph': glyph, 'char': char,
                         'liga': -1, 'children': []}
                children.append(child)
            children = child['children']
        child['liga'] = liga

    for (chars, liga,) in ligas:
        add_node(
            map(lambda char: (glyph_name_id_map[char], char,), chars), liga)

    def sort_children(children):
        for child in children:
            child['children'] = sort_children(child['children'])
        return sorted(children, key=lambda x: x['glyph'])

    forest = sort_children(forest)

    def write_node(node):
        f.write(struct.pack('!H', node['glyph']))
        f.write(struct.pack('!i', node['liga']))
        f.write(struct.pack('!H', len(node['children'])))
        for child in node['children']:
            write_node(child)

    root = {'glyph': 0, 'char': 0, 'liga': -1, 'children': forest}
    write_node(root)


def write_math_consts(f, consts):
    for v in consts:
        f.write(struct.pack('!h', v))


def write_glyphs(f, glyphs, glyph_name_id_map, is_math_font, have_glyph_path):
    f.write(struct.pack('!H', len(glyphs)))

    def write_metrics(metrics):
        for v in metrics:
            f.write(struct.pack('!h', int(v)))

    def write_kerns(g, kerns):
        if not kerns:
            f.write(struct.pack('!H', 0))
            return
        f.write(struct.pack('!H', len(list(kerns))))
        sorted_kerns = chain(
            partial(map, lambda x: (glyph_name_id_map[x[0]], x[1],)),
            partial(sorted, key=lambda x: x[0])
        )(kerns)
        for (glyph, value,) in sorted_kerns:
            f.write(struct.pack('!Hh', glyph, value))

    def write_variants(variants):
        length = 0 if not variants else len(variants)
        f.write(struct.pack('!H', length))
        if length == 0:
            return
        for v in variants:
            gid = glyph_name_id_map[v]
            f.write(struct.pack('!H', gid))

    def write_glyph_assembly(assembly):
        if not assembly or not assembly[1]:
            f.write(struct.pack('?', False))
            return
        f.write(struct.pack('?', True))
        f.write(struct.pack('!H', len(assembly[1])))
        f.write(struct.pack('!h', assembly[0]))  # italics correction
        for part in assembly[1]:
            f.write(struct.pack('!H', glyph_name_id_map[part[0]]))
            for v in part[1:]:
                f.write(struct.pack('!H', v))

    def write_math_kern(math_kerns):
        for i in math_kerns:
            if not i:
                f.write(struct.pack('!H', 0))
                continue
            f.write(struct.pack('!H', len(i)))
            for (correction_height, value,) in i:
                f.write(struct.pack('!hh', correction_height, value))

    def write_math(math):
        f.write(struct.pack('!h', math[0]))  # italics correction
        f.write(struct.pack('!h', math[1]))  # topaccent attachment
        write_variants(math[2])  # horizontal variants
        write_variants(math[3])  # vertical variants
        write_variants(math[4])  # scripts variants
        write_glyph_assembly(math[5])  # horizontal assembly
        write_glyph_assembly(math[6])  # vertical assembly
        write_math_kern(math[7])  # math kern

    def write_path(path):
        f.write(struct.pack('!H', len(path)))
        if not path:
            return
        for cmd in path:
            f.write(struct.pack('c', bytes(cmd[0], 'ascii')))
            for param in cmd[1:]:
                f.write(struct.pack('!h', param))

    for glyph in glyphs:
        write_metrics(glyph[0])
        write_kerns(glyph[4], glyph[1])
        if is_math_font:
            write_math(glyph[2])
        if have_glyph_path:
            write_path(glyph[3])


def parse_fontstyle(font, userstyle):
    NONE = 0b00000000
    RM =   0b00000001
    BF =   0b00000010
    IT =   0b00000100
    CAL =  0b00001000
    FRAK = 0b00010000
    BB =   0b00100000
    SF =   0b01000000
    TT =   0b10000000

    style = NONE

    if userstyle is None or userstyle == "":
        if font.os2_weight >= 700:
            style |= BF

        if font.os2_stylemap == 0x40:
            style |= RM
        if font.os2_stylemap == 0x21:
            style |= (BF | IT)
        if font.os2_stylemap == 0x20:
            style |= BF
        if font.os2_stylemap == 0x01:
            style |= IT

        if font.macstyle != -1:
            if font.macstyle & 0b01 == 0b01:
                style |= BF
            if font.macstyle & 0b10 == 0b10:
                style |= IT

        if font.fullname.upper().__contains__("MONO"):
            style |= TT
    else:
        userstyles = userstyle.split(",")
        if "rm" in userstyles:
            style |= RM
        if "bf" in userstyles:
            style |= BF
        if "it" in userstyles:
            style |= IT
        if "cal" in userstyles:
            style |= CAL
        if "frak" in userstyles:
            style |= FRAK
        if "bb" in userstyles:
            style |= BB
        if "sf" in userstyles:
            style |= SF
        if "tt" in userstyles:
            style |= TT

    return style


def parse_otf(file_path, have_glyph_path, output_file_path, userstyle=""):
    print("parsing font " + file_path + ", please wait...")
    font = fontforge.open(file_path)

    is_math_font = font.math.exists()
    # read math constants
    math_consts = []
    if is_math_font:
        math_consts = read_math_consts(font)

    # read kern subtables
    kern_subtable_names = read_kern_subtables(font)
    # read ligature subtables
    liga_subtable_names = read_ligature_subtables(font)
    # read kern class tables
    kern_class_tables = read_kerning_class(font)
    # read scripts subtables
    scripts_subtable_names = read_scripts_subtables(font)

    unicode_glyph_map = []
    glyph_name_id_map = {}
    glyphs = []
    ligas = []

    # read glyphs in GID order
    for glyph_name in font:
        glyph = font[glyph_name]

        # unicode-glyph map
        if glyph.unicode != -1:
            unicode_glyph_map.append((glyph.unicode, glyph.originalgid,))

        glyph_name_id_map[glyph_name] = glyph.originalgid
        # glyph info
        glyphs.append(read_glyph(
            glyph, is_math_font, have_glyph_path,
            kern_subtable_names, scripts_subtable_names
        ))

        # read ligature
        liga_info = read_ligatures(glyph, liga_subtable_names)
        for l in liga_info:
            ligas.append(l)

    name = font.fullname
    family = font.familyname
    style = parse_fontstyle(font, userstyle)
    em = font.em
    xheight = font.xHeight
    ascent = font.ascent
    descent = font.descent

    with open(output_file_path, 'wb') as f:
        f.write(struct.pack('c', bytes('c', 'ascii')))
        f.write(struct.pack('c', bytes('l', 'ascii')))
        f.write(struct.pack('c', bytes('m', 'ascii')))
        # current major version 5
        f.write(struct.pack('!H', 5))
        # minor version, if support glyph path
        f.write(struct.pack('B', 2 if have_glyph_path else 1))
        f.write(struct.pack(str(len(name) + 1) + 's', bytes(name, 'utf-8')))
        f.write(struct.pack(str(len(family) + 1) + 's', bytes(family, 'utf-8')))
        f.write(struct.pack('?', is_math_font))
        f.write(struct.pack('!H', style))
        f.write(struct.pack('!H', em))
        f.write(struct.pack('!H', int(xheight)))
        f.write(struct.pack('!H', ascent))
        f.write(struct.pack('!H', descent))
        write_clm_unicode_glyph_map(f, unicode_glyph_map)
        write_clm_kerning_class(f, kern_class_tables, glyph_name_id_map)
        write_ligas(f, ligas, glyph_name_id_map)
        if is_math_font:
            write_math_consts(f, math_consts)
        write_glyphs(f, glyphs, glyph_name_id_map,
                     is_math_font, have_glyph_path)

    font.close()


def batch_parse(input_dir, have_glyph_path, output_dir):
    import os
    fs = os.listdir(input_dir)
    if not os.path.isdir(output_dir):
        os.makedirs(output_dir)
    for f in fs:
        if f.endswith('.otf'):
            name = os.path.basename(f)[0:-4]
            save_name = name + ".clm2" if have_glyph_path else name + ".clm1"
            save_path = os.path.join(output_dir, save_name)
            input_file = os.path.join(input_dir, f)
            parse_otf(input_file, have_glyph_path, save_path)
    print("The generated clm data files were saved into directory: " + output_dir)


def single_parse(input_file, have_glyph_path, output_dir, user_style):
    import os
    name = os.path.basename(input_file)[0:-4]
    save_name = name + ".clm2" if have_glyph_path else name + ".clm1"
    save_path = os.path.join(output_dir, save_name)
    parse_otf(input_file, have_glyph_path, save_path, user_style)
    print("The generated clm data file was saved into directory: " + output_dir);


usage = """
Generate clm data from OTF font file.

Usage:
    with single mode:
        --single \\
        <path/to/OTF-font> \\
        <if_parse_glyph_path: true | false> \\
        <dir/to/save/clm_file> \\
        <font-style list, split by ','>

    or with batch mode:
        --batch \\
        <dir/to/OTF-fonts> \\
        <if_parse_glyph_path: true | false> \\
        <dir/to/save/clm_files>

    The batch mode will not run in recursive, allowed font styles are:
        rm: roman
        bf: bold
        it: italic
        cal: calligraphic
        frak: fraktur
        bb: double struck
        sf: sans-serif
        tt: type-writer

    If no font styles are given, trying to read it from font, but only
    the following styles will be taken into account:
        rm, bf, it, tt
"""


def main():
    if len(sys.argv) < 4:
        print(usage)
        return
    if sys.argv[1] == '--batch':
        batch_parse(
            sys.argv[2],
            sys.argv[3] == 'true',
            sys.argv[4]
        )
    else:
        userstyle = ""
        if (len(sys.argv) > 5):
            userstyle = sys.argv[5]
        single_parse(
            sys.argv[2],
            sys.argv[3] == 'true',
            sys.argv[4],
            userstyle
        )


if __name__ == "__main__":
    main()
