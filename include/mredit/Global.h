#pragma once

#include <qglobal.h>

//struct QMetaObject;

namespace mredit {
namespace Global {

    enum class Ruler {
        NoRuler = -1,
        LineRuler,
        BackgroundRuler
    };

    // the order is important as it defined the presentation order of the margins
    enum class Margin {
        InvalidMargin = -1,
        BookmarkMargin,
        NumberMargin,
        FoldMargin,
        RevisionMargin,
        SpaceMargin,
		CustomMargin,
    };

    enum class Rule {
        InvalidRule = -1,
        KeywordRule,
        FloatRule,
        HlCOctRule,
        HlCHexRule,
        HlCFloatRule,
        IntRule,
        DetectCharRule,
        Detect2CharsRule,
        AnyCharRule,
        StringDetectRule,
        WordDetectRule,
        RegExprRule,
        LineContinueRule,
        HlCStringCharRule,
        RangeDetectRule,
        HlCCharRule,
        DetectSpacesRule,
        DetectIdentifierRule,
        IncludeRulesRule
    };

    enum class DefaultStyle {
        InvalidStyle = -1,
        NormalStyle,
        KeywordStyle,
        DataTypeStyle,
        DecValStyle,
        BaseNStyle,
        FloatStyle,
        CharStyle,
        StringStyle,
        CommentStyle,
        OthersStyle,
        AlertStyle,
        FunctionStyle,
        RegionMarkerStyle,
        ErrorStyle
    };

    //const QMetaObject* metaObject();
} } // mredit::Global

