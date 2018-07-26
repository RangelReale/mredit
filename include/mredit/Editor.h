#pragma once

#include <QPlainTextEdit>

#include <mredit/Global.h>
#include <mredit/BookmarkGroup.h>

namespace mredit {

class CodeEditorPrivate;
class TextDocument;
namespace margin {
class MarginStacker;
class MarginStackerPrivate;
}

class Editor : public QPlainTextEdit
{
    Q_OBJECT
    Q_ENUMS(IndentationPolicy)
    friend class CodeEditorPrivate;
    friend class margin::MarginStacker;
    friend class margin::MarginStackerPrivate;

public:
    Editor( QWidget* parent = 0 );
    virtual ~Editor();

    enum IndentationPolicy {
        UseTabs,
        UseSpaces
    };
    
    TextDocument* textDocument() const;
    void setTextDocument( TextDocument* document );
    
	margin::MarginStacker* marginStacker() const;
    void setMarginStacker(margin::MarginStacker* marginStacker );
    
    QString text() const;
    QString text(int line) const;

    Editor::IndentationPolicy indentationPolicy();
    int indentationWidth();

    QPoint cursorPosition() const;
    int lines() const;
    int currentLine() const;
    int currentColumn() const;
    Global::Ruler rulerMode() const;
    int rulerWidth() const;
	void moveCursorToLine(int line, int col = -1);
	void ensureLineVisible(int line, int col = -1);
	void ensureLineCenter(int line, int col = -1);

    QBrush paper() const;
    QBrush pen() const;
    QBrush selectionBackground() const;
    QBrush selectionForeground() const;
    QBrush caretLineBackground() const;
    QBrush caretLineForeground() const;
    
	BookmarkGroupList &bookmarkGroups();

    bool hasBookmark(int bmgroup, const QTextBlock& block ) const;
    bool hasBookmark(int bmgroup, int line ) const;
    
    QRect blockRect( const QTextBlock& block ) const;
    QRect lineRect( int line ) const;
public slots:
    void setText( const QString& text );
    void setText( int line, const QString &text );
    void setInitialText( const QString& text );
	void openFile(const QString& filename);

    void setCursorPosition( const QPoint& pos );
    void setCurrentLine( int line );
    void setCurrentColumn( int column );
    void setRulerMode( Global::Ruler mode );
    void setRulerWidth( int width );
    
    void setPaper( const QBrush& brush );
    void setPen( const QBrush& brush );
    void setSelectionBackground( const QBrush& brush );
    void setSelectionForeground( const QBrush& brush );
    void setCaretLineBackground( const QBrush& brush );
    void setCaretLineForeground( const QBrush& brush );
    
    void setBookmark(int bmgroup, const QTextBlock& block, bool set );
    void setBookmark(int bmgroup, int line, bool set );
    void toggleBookmark(int bmgroup, const QTextBlock& block );
    void toggleBookmark(int bmgroup, int line );
	void clearBookmarks(int bmgroup);

    void indent();
    void unindent();

    void setTabWidth(int size);

    void setIndentationPolicy(Editor::IndentationPolicy policy);
    void setIndentationWidth(int width);

    void insertTab();
    void removeTab();

    void insertLine(int after = -1);
    void removeLine(int line = -1);
    void duplicateLine(int line = -1);

    void expandSelectionToLine();
    void expandSelectionToWord();

    void joinLines();
    void swapLines(int first, int second);
    void swapLineUp();
    void swapLineDown();

protected:
	bool event(QEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
	void keyPressEvent(QKeyEvent *event) override;

private:
    CodeEditorPrivate* d;
};

}
