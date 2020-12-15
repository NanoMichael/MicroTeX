#!/usr/bin/python2
# -*- coding: utf-8 -*-

# Parse open-type font file and convert to `clm` format file

import sys
from functools import reduce, partial
import fontforge


def chain(*fs):
    def _chain(f, g):
        return lambda x: g(f(x))
    return reduce(_chain, fs, lambda x: x)


def fmap(f, it):
    return [j for i in it for j in f(i)]


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
    bounding_box = glyph.boundingBox()
    return (
        glyph.width,
        bounding_box[2],
        -bounding_box[1]
    )


def read_variants(get_variants):
    '''
    Return a array of glyph names to represents variants
    '''
    variants = get_variants()
    if not variants:
        return []
    return variants.split(' ')


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


def read_math(glyph):
    return (
        glyph.italicCorrection,
        glyph.topaccent,
        read_variants(lambda: glyph.horizontalVariants),
        read_variants(lambda: glyph.verticalVariants),
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
    return chain(
        partial(fmap, lambda subtable_name: glyph.getPosSub(subtable_name)),
        # only care about horizontal kerning
        partial(filter, lambda kern_info: kern_info[5] != 0),
        partial(map, lambda kern_info: (kern_info[2], kern_info[5],))
    )(kern_subtable_names)


def read_glyph(glyph, is_math_font, kern_subtable_names):
    '''
    Return a tuple
    (
        metrics,
        kerning,
        math
    )
    '''
    return (
        read_metrics(glyph),
        read_kern(glyph, kern_subtable_names),
        None if not is_math_font else read_math(glyph),
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


def read_kern_subtables(font):
    '''
    Return array of kerning subtable name
    '''
    return _read_lookup_subtables(
        font,
        lambda lookup_info: lookup_info[0] == 'gpos_pair',
        lambda subtable_name: not font.isKerningClass(subtable_name)
    )(font.gpos_lookups)


def read_ligature_subtables(font):
    '''
    Return array of ligture subtable name
    '''
    return _read_lookup_subtables(
        font,
        lambda lookup_info: lookup_info[0] == 'gsub_ligature',
    )(font.gsub_lookups)


def read_ligatures(glyph, liga_subtable_names):
    '''
    Return array of ligatures info represents by this glyph
    [
        ((glyph_name, glyph_name, ...), glyph_id),
        ...
    ]
    '''
    get_ligas = chain(
        partial(fmap, lambda subtable_name: glyph.getPosSub(subtable_name)),
        partial(map, lambda liga_info: (liga_info[2:], glyph.originalgid,))
    )
    return get_ligas(liga_subtable_names)


def read_kerning_class(font):
    '''
    Return array of kerning class table:
    (
        # glyphs on left
        (
            (glyph_name, glyph_name, ...),
            ...
        ),
        # glyphs on right
        (
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
    get_tables = chain(
        _read_lookup_subtables(
            font,
            lambda lookup_info: lookup_info[0] == 'gpos_pair',
            lambda subtable_name: font.isKerningClass(subtable_name)
        ),
        partial(map, lambda subtable_name: font.getKerningClass(subtable_name))
    )
    return get_tables(font.gpos_lookups)


def parse_otf():
    font = fontforge.open(sys.argv[1])
    is_math_font = sys.argv[2] == 'true'

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

    unicode_glyph_map = []
    glyphs = []
    ligas = []

    # read glyphs in GID order
    for glyph_name in font:
        glyph = font[glyph_name]
        if glyph.unicode != -1:
            unicode_glyph_map.append((glyph.unicode, glyph.originalgid,))
        glyphs.append(read_glyph(glyph, is_math_font, kern_subtable_names))

        # read ligature
        liga_info = read_ligatures(glyph, liga_subtable_names)
        for l in liga_info:
            ligas.append(l)

    for i in kern_class_tables:
        print(i)

    font.close()


if __name__ == "__main__":
    parse_otf()
