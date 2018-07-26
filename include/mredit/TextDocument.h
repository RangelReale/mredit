#pragma once

#include <QTextDocument>

namespace mredit {

class TextDocumentPrivate;
class PlainTextDocumentLayout;
class TextBlockUserData;

//namespace Syntax {
    //class Highlighter;
//}

class TextDocument : public QTextDocument
{
    Q_OBJECT
    friend class TextDocumentPrivate;

public:
    TextDocument( QObject* parent = 0 );
    virtual ~TextDocument();

    QTextBlock findBlockByContainsPosition(int position);

    PlainTextDocumentLayout* layout() const;
    void setLayout( PlainTextDocumentLayout* layout );

    //Syntax::Highlighter* syntaxHighlighter() const;
    //void setSyntaxHighlighter( Syntax::Highlighter* highlighter );

    TextBlockUserData* testUserData( const QTextBlock& block ) const;
    TextBlockUserData* userData( QTextBlock& block ) const;

    QString text() const;
    void setText( const QString& text );

    void setInitialText( const QString& text );

    int lastUnmodifiedRevision() const;

private:
    TextDocumentPrivate* d;
};

}
