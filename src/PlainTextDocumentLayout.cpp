#include "mredit/PlainTextDocumentLayout.h"
#include "mredit/TextDocument.h"

namespace mredit {

PlainTextDocumentLayout::PlainTextDocumentLayout( TextDocument* document )
    : QPlainTextDocumentLayout( document )
{
}

PlainTextDocumentLayout::~PlainTextDocumentLayout()
{
}

}