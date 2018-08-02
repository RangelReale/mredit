#include "mredit/Editor.h"
#include "mredit/TextDocument.h"
#include "mredit/TextBlockUserData.h"
#include "mredit/margin/MarginStacker.h"
#include "mredit/margin/AbstractMargin.h"
#include "mredit/margin/LineNumberMargin.h"

#include <QMessageBox>
#include <QStyleOptionFrame>
#include <QKeyEvent>
#include <QTextBlock>
#include <QPainter>
#include <QDebug>
#include <QFontMetrics>
#include <QApplication>

namespace mredit {

// CodeEditorPrivate

class CodeEditorPrivate {
public:
    Editor* editor;
    margin::MarginStacker* stacker;
    QPalette originalPalette;
    Global::Ruler rulerMode;
    int rulerWidth;
	BookmarkGroupList bookmarkgroups;
	bool highlightCaretLine;

    Editor::IndentationPolicy indentationPolicy;
    int indentationWidth;
    
    CodeEditorPrivate( Editor* _editor )
            : editor( _editor ),
            stacker( 0 ),
            originalPalette( _editor->palette() ),
            rulerMode(Global::Ruler::NoRuler ),
            rulerWidth( 80 ),
			bookmarkgroups(),
            indentationWidth(4),
			highlightCaretLine(true)
    {
        Q_ASSERT( editor );
    }

    QLine rulerLine() const {
        const QPoint offset = editor->contentOffset().toPoint();
        const int x = rulerWidth *QFontMetrics( editor->font() ).averageCharWidth();
        return QLine( QPoint( x +offset.x(), 0 ), QPoint( x +offset.x(), editor->viewport()->height() ) );
    }
    
    QRect rulerRect() const {
        const QPoint offset = editor->contentOffset().toPoint();
        const int x = rulerWidth *QFontMetrics( editor->font() ).averageCharWidth();
        QRect rect( QPoint( x +offset.x(), 0 ), QSize( editor->viewport()->size() -QSize( x +offset.x(), 0 ) ) );
        return rect;
    }
    
    QRect caretLineRect() const {
        QRect rect = editor->cursorRect()/*.adjusted( 0, -1, 0, 1 )*/;
        rect.setX( 0 );
        rect.setWidth( editor->viewport()->width() );
        return rect;
    }
    
    void paintFrame() {
        QPainter painter( editor );
        QStyleOptionFrame option;
        
        option.initFrom( editor );
        option.palette = originalPalette;
        option.rect = editor->frameRect();
        option.frameShape = editor->frameShape();
        
        switch ( option.frameShape ) {
            case QFrame::Box:
            case QFrame::HLine:
            case QFrame::VLine:
            case QFrame::StyledPanel:
            case QFrame::Panel:
                option.lineWidth = editor->lineWidth();
                option.midLineWidth = editor->midLineWidth();
                break;
            default:
                // most frame styles do not handle customized line and midline widths
                // (see updateFrameWidth()).
                option.lineWidth = editor->frameWidth();
                break;
        }

        if ( editor->frameShadow() == QFrame::Sunken ) {
            option.state |= QStyle::State_Sunken;
        }
        else if ( editor->frameShadow() == QFrame::Raised ) {
            option.state |= QStyle::State_Raised;
        }

        editor->style()->drawControl( QStyle::CE_ShapedFrame, &option, &painter, editor );
    }
};

// CodeEditor

Editor::Editor( QWidget* parent )
    : QPlainTextEdit( parent ),
        d( new CodeEditorPrivate( this ) )
{
    setTextDocument( new TextDocument( this ) );
    setAutoFillBackground( true );
    setCaretLineBackground( caretLineBackground().color().lighter( 200 ) );

	d->stacker = new margin::MarginStacker(this);

    // As default
    setIndentationWidth(4);
    setIndentationPolicy(UseSpaces);
    
    connect( this, SIGNAL( cursorPositionChanged() ), viewport(), SLOT( update() ) );
}

Editor::~Editor()
{
    delete d;
}

TextDocument* Editor::textDocument() const
{
    return qobject_cast<TextDocument*>( document() );
}

void Editor::setTextDocument( TextDocument* document )
{
    setDocument( document );
}

margin::MarginStacker* Editor::marginStacker() const
{
    return d->stacker;
}

void Editor::setMarginStacker(margin::MarginStacker* marginStacker )
{
    if ( d->stacker == marginStacker ) {
        return;
    }
    
    if ( d->stacker ) {
        d->stacker->deleteLater();
    }
    
    d->stacker = marginStacker;
    
    if ( d->stacker ) {
        d->stacker->setEditor( this );
    }
}

QString Editor::text() const
{
    return textDocument()->text();
}

QString Editor::text(int line) const
{
    return textDocument()->findBlockByNumber(line).text();
}

Editor::IndentationPolicy Editor::indentationPolicy()
{
    return d->indentationPolicy;
}

int Editor::indentationWidth()
{
    return d->indentationWidth;
}

void Editor::setIndentationPolicy(Editor::IndentationPolicy policy)
{
    d->indentationPolicy = policy;
    if(policy == UseTabs)
        setTabWidth(d->indentationWidth);
}

void Editor::setIndentationWidth(int width)
{
    d->indentationWidth = width;
}

void Editor::setTabWidth(int size)
{
    QFontMetrics metrics(font());
    setTabStopWidth((metrics.charWidth("W", 0)*size) / 2);
}

void Editor::insertTab()
{
    QTextCursor cursor = textCursor();

    if (d->indentationPolicy == UseSpaces) {
        QString spaces(d->indentationWidth, ' ');
        cursor.insertText(spaces);
    } else {
        cursor.insertText("\t");
    }

    setTextCursor(cursor);
}

void Editor::removeTab()
{
    const int width = d->indentationWidth;

    QTextCursor cursor = textCursor();

    if (d->indentationPolicy == UseSpaces) {
        if(currentColumn() - width > width) {
            cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, width);
        } else {
            cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
        }
        cursor.removeSelectedText();
    } else {
        cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, 1);
        cursor.removeSelectedText();
    }

    setTextCursor(cursor);
}

void Editor::indent()
{
    QPoint position = cursorPosition();
    QTextCursor cursor = textCursor();

    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    setTextCursor(cursor);
    insertTab();

    if(d->indentationPolicy == UseSpaces) {
        position.setX( position.x()+d->indentationWidth );
    } else {
        position.setX( position.x()+1 );
    }

    setCursorPosition(position);
}

void Editor::unindent()
{
    QPoint position = cursorPosition();

    QTextCursor cursor = textCursor();
    QString line = text(currentLine());
    int logicalBegin = 0;
    for(int k = 0; k != line.size(); k++) {
        if(line[k] == ' ' || line[k] == '\t') {
            logicalBegin++;
        } else {
            break;
        }
    }
    if(logicalBegin == 0)
        return;
    //qDebug() << logicalBegin;
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    if(d->indentationPolicy == UseSpaces) {
        cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, d->indentationWidth);
    } else {
        cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 1);
    }
    setTextCursor(cursor);
    removeTab();

    if(d->indentationPolicy == UseSpaces) {
        position.setX( position.x()-d->indentationWidth );
    } else {
        position.setX( position.x()-1 );
    }
    setCursorPosition(position);
}

void Editor::setText( const QString& text )
{
    textDocument()->setText(text);
    moveCursor(QTextCursor::Start, QTextCursor::MoveAnchor);
}

void Editor::setText( int line, const QString &text )
{
    setCurrentLine(line);
    setCurrentColumn(0);
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    cursor.insertText(text);
    setTextCursor(cursor);
}

void Editor::insertLine( int after )
{
    if (after < 0)
        after = currentLine();
    if (after > (lines()-1))
        after = firstVisibleBlock().blockNumber();

    setCurrentLine(after);
    setCurrentColumn(0);
    QTextCursor c = textCursor();
    c.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
    c.insertText("\n");
    setTextCursor(c);
}

void Editor::removeLine( int line )
{
    if (line < 0)
        line = currentLine();
    if (line > (lines()-1))
        line = firstVisibleBlock().blockNumber();

    setCurrentLine(line);
    setCurrentColumn(0);
    QTextCursor c = textCursor();
    QTextBlock block = textDocument()->findBlockByNumber(line);
    c.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, block.length());
    c.removeSelectedText();
    setTextCursor(c);
}

void Editor::duplicateLine(int line)
{
    if (line < 0)
        line = currentLine();
    QPoint nat = cursorPosition();

    setCurrentLine(line);
    QTextCursor curz = textCursor();
    curz.movePosition(QTextCursor::EndOfLine, QTextCursor::MoveAnchor);
    curz.insertText("\n"+text(line));

    nat.setY(nat.y()+2);
    setCursorPosition(nat);
}

void Editor::expandSelectionToLine()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor);
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    setTextCursor(cursor);
}

void Editor::expandSelectionToWord()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::WordRight, QTextCursor::KeepAnchor);
    cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    setTextCursor(cursor);
}

void Editor::joinLines()
{
    const int selectionStart = textCursor().selectionStart();
    const int selectionEnd = textCursor().selectionEnd();
    int start = textDocument()->findBlockByContainsPosition(selectionStart).blockNumber();
    int end   = textDocument()->findBlockByContainsPosition(selectionEnd).blockNumber();

    if (start == end)
        return;
    if (start > end)
        qSwap(start, end);

    QString buffer = text(start);
    buffer += " ";
    for(int k = start+1; k <= end; k++) {
        QString currentText = text(start+1);
        currentText.remove("\t");
        currentText += " ";
        buffer += currentText;
        removeLine(start+1);
    }
    buffer.remove(buffer.size()-1, 1);
    setText(start, buffer);
}

void Editor::swapLines(int first, int second)
{
    if ((first < 0 || first >= lines()) || (second < 0 || second >= lines()))
        return;

    QPoint nat = cursorPosition();
    nat.setY( nat.y() + (second-first) );

    QString temp = text(first);
    setText(first, text(second));
    setText(second, temp);

    setCursorPosition(nat);
}

void Editor::swapLineUp()
{
    swapLines(currentLine(), currentLine()-1);
}

void Editor::swapLineDown()
{
    swapLines(currentLine(), currentLine()+1);
}

void Editor::setInitialText( const QString& text )
{
    textDocument()->setInitialText( text );
    moveCursor( QTextCursor::Start, QTextCursor::MoveAnchor );
}

void Editor::openFile(const QString& filename)
{
	QFile file(filename);
	if (file.open(QFile::ReadOnly)) {
		setPlainText(file.readAll());
	}
}

QPoint Editor::cursorPosition() const
{
    const QTextCursor cursor = textCursor();
    return cursor.isNull() ? QPoint() : QPoint( cursor.positionInBlock(), cursor.blockNumber() );
}

void Editor::setCursorPosition( const QPoint& pos )
{
    const QTextBlock block = textDocument()->findBlockByLineNumber( pos.y() );
    const int position = block.position() +( pos.x() < block.length() ? pos.x() : 0 );
    QTextCursor cursor = textCursor();
    cursor.setPosition( position, QTextCursor::MoveAnchor );
    setTextCursor( cursor );
}

int Editor::lines() const
{
    return textDocument()->blockCount();
}

int Editor::currentLine() const
{
    return cursorPosition().y();
}

void Editor::setCurrentLine( int line )
{
    setCursorPosition( QPoint( currentColumn(), line ) );
}

int Editor::currentColumn() const
{
    return cursorPosition().x();
}

void Editor::setCurrentColumn( int column )
{
    setCursorPosition( QPoint( column, currentLine() ) );
}

Global::Ruler Editor::rulerMode() const
{
    return d->rulerMode;
}

void Editor::setRulerMode(Global::Ruler mode )
{
    d->rulerMode = mode;
    viewport()->update();
}

int Editor::rulerWidth() const
{
    return d->rulerWidth;
}

void Editor::moveCursorToLine(int line, int col)
{
	QTextCursor cursor(textDocument()->findBlockByNumber(line - 1));
	if (col != -1)
		cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, col);
	setTextCursor(cursor);
}

void Editor::ensureLineVisible(int line, int col)
{
	moveCursorToLine(line, col);
	ensureCursorVisible();
}

void Editor::ensureLineCenter(int line, int col)
{
	moveCursorToLine(line, col);
	centerCursor();
}

void Editor::setRulerWidth( int width )
{
    d->rulerWidth = width;
    viewport()->update();
}

QBrush Editor::paper() const
{
    return palette().brush( viewport()->backgroundRole() );
}

void Editor::setPaper( const QBrush& brush )
{
    QPalette pal = palette();
    pal.setBrush( viewport()->backgroundRole(), brush );
    setPalette( pal );
}

QBrush Editor::pen() const
{
    return palette().brush( viewport()->foregroundRole() );
}

void Editor::setPen( const QBrush& brush )
{
    QPalette pal = palette();
    pal.setBrush( viewport()->foregroundRole(), brush );
    setPalette( pal );
}

QBrush Editor::selectionBackground() const
{
    return palette().brush( QPalette::Highlight );
}

void Editor::setSelectionBackground( const QBrush& brush )
{
    QPalette pal = palette();
    pal.setBrush( QPalette::Highlight, brush );
    setPalette( pal );
}

QBrush Editor::selectionForeground() const
{
    return palette().brush( QPalette::HighlightedText );
}

void Editor::setSelectionForeground( const QBrush& brush )
{
    QPalette pal = palette();
    pal.setBrush( QPalette::HighlightedText, brush );
    setPalette( pal );
}

QBrush Editor::caretLineBackground() const
{
    return palette().brush( QPalette::Link );
}

void Editor::setCaretLineBackground( const QBrush& brush )
{
    QPalette pal = palette();
    pal.setBrush( QPalette::Link, brush );
    setPalette( pal );
}

QBrush Editor::caretLineForeground() const
{
    return palette().brush( QPalette::LinkVisited );
}

void Editor::setCaretLineForeground( const QBrush& brush )
{
    QPalette pal = palette();
    pal.setBrush( QPalette::LinkVisited, brush );
    setPalette( pal );
}

BookmarkGroupList &Editor::bookmarkGroups()
{
	return d->bookmarkgroups;
}

bool Editor::hasBookmark(int bmgroup, const QTextBlock& block ) const
{
    const TextBlockUserData* data = textDocument()->testUserData( block );
    return data ? data->bookmarkgroups.contains(bmgroup) : false;
}

void Editor::setBookmark(int bmgroup, const QTextBlock& block, bool set )
{
    const TextDocument* document = textDocument();
    TextBlockUserData* data = document->testUserData( block );
    
    if ( !data && !set ) {
        return;
    }

	BookmarkGroup *group = d->bookmarkgroups.getGroup(bmgroup);
	if (!bmgroup) {
		return;
	}

    data = document->userData( *const_cast<QTextBlock*>( &block ) );
	if (!data) {
		return;
	}

	if (set)
		data->bookmarkgroups.insert(bmgroup);
	else
		data->bookmarkgroups.remove(bmgroup);

	if (set && group->format().isValid()) {
		QTextCursor(block).setBlockFormat(group->format());
	}
	else {
		QTextCursor(block).setBlockFormat(QTextBlockFormat());
	}

    if ( d->stacker ) {
		auto bms = d->stacker->margins(Global::Margin::BookmarkMargin);
		margin::AbstractMargin* margin;
		foreach(margin, bms) {
			margin->updateLineRect(block.blockNumber());
		}
    }
}

void Editor::toggleBookmark(int bmgroup, const QTextBlock& block )
{
    setBookmark(bmgroup, block, !hasBookmark(bmgroup, block ) );
}

bool Editor::hasBookmark(int bmgroup, int line ) const
{
    return hasBookmark(bmgroup, textDocument()->findBlockByNumber( line ) );
}

void Editor::setBookmark(int bmgroup, int line, bool set )
{
    setBookmark(bmgroup, textDocument()->findBlockByNumber( line ), set );
}

void Editor::toggleBookmark(int bmgroup, int line )
{
    toggleBookmark(bmgroup, textDocument()->findBlockByNumber( line ) );
}

void Editor::clearBookmarks(int bmgroup)
{
	const TextDocument* document = textDocument();
	QTextBlock currentBlock = document->firstBlock();
	while (currentBlock.isValid()) {
		TextBlockUserData* data = document->testUserData(currentBlock);
		if (data) {
			if (data->bookmarkgroups.contains(bmgroup)) {
				setBookmark(bmgroup, currentBlock, false);
			}
		}
		currentBlock = currentBlock.next();
	}
}

QRect Editor::blockRect( const QTextBlock& block ) const
{
    QRectF rect = blockBoundingGeometry( block );
    rect.moveTopLeft( rect.topLeft() +QPointF( 0, contentOffset().y() ) );
    return rect.toRect();
}

QRect Editor::lineRect( int line ) const
{
    return blockRect( textDocument()->findBlockByNumber( line ) );
}

bool Editor::isHighlightCaretLine() const
{
	return d->highlightCaretLine;
}

void Editor::setHighlightCaretLine(bool value)
{
	d->highlightCaretLine = value;
}

void Editor::setLabelLayout(bool value)
{
	if (value)
	{
		QPalette readOnlyPalette = palette();
		QColor mainWindowBgColor = palette().color(QPalette::Window);
		readOnlyPalette.setColor(QPalette::Base, mainWindowBgColor);
		setPalette(readOnlyPalette);

		setHighlightCaretLine(false);
		setFrameShape(QFrame::NoFrame);
	}
	else
	{
		// TODO: revert palette

		setHighlightCaretLine(true);
	}
}

bool Editor::event( QEvent* event )
{
    switch ( event->type() ) {
        case QEvent::QEvent::Paint:
            d->paintFrame();
            return true;
        default:
            break;
    }
    
    return QPlainTextEdit::event( event );
}

void Editor::paintEvent( QPaintEvent* event )
{
	if (d->highlightCaretLine)
	{
		QPainter painter(viewport());
		painter.setRenderHint(QPainter::Antialiasing, false);

		// draw ruler
		switch ( d->rulerMode ) {
			case Global::Ruler::NoRuler:
				break;
			case Global::Ruler::LineRuler:
				painter.setPen( QPen( caretLineForeground(), painter.pen().widthF() ) );
				painter.drawLine( d->rulerLine() );
				break;
			case Global::Ruler::BackgroundRuler:
				painter.setPen( Qt::NoPen );
				painter.setBrush( caretLineForeground() );
				painter.drawRect( d->rulerRect() );
				break;
		}
    
		// draw caret line
		painter.fillRect(d->caretLineRect(), caretLineBackground());
		painter.end();
	}
    
    // normal editor painting
    QPlainTextEdit::paintEvent( event );
}

void Editor::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Tab) {
        if (event->modifiers() == Qt::NoModifier) {
            insertTab();
            return;
        } else if (event->modifiers() == Qt::ShiftModifier) {
            removeTab();
            return;
        }
    }

    if(d->stacker->margin(Global::Margin::NumberMargin))
        d->stacker->margin(Global::Margin::NumberMargin)->update();

    QPlainTextEdit::keyPressEvent(event);
}

}