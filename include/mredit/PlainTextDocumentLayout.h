#pragma once

#include <QPlainTextDocumentLayout>

namespace mredit {

class TextDocument;

class PlainTextDocumentLayout : public QPlainTextDocumentLayout
{
    Q_OBJECT
    
public:
    PlainTextDocumentLayout( TextDocument* document );
    virtual ~PlainTextDocumentLayout();
};

}