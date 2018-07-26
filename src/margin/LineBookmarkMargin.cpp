#include "mredit/margin/LineBookmarkMargin.h"
#include "mredit/margin/MarginStacker.h"
#include "mredit/Editor.h"
#include "mredit/TextDocument.h"
#include "mredit/TextBlockUserData.h"

#include <QPainter>
#include <QTextBlock>
#include <QPixmapCache>
#include <QIcon>
#include <QDebug>
#include <QSet>

namespace mredit {
namespace margin {

#define LineBookmarkMarginMargins 1

// LineBookmarkMarginPrivate

class LineBookmarkMarginPrivate : public QObject {
    Q_OBJECT

public:
    LineBookmarkMargin* margin;
	QSet<int> bookmarkgroups;
	int bmgroup_first;
    
    LineBookmarkMarginPrivate( LineBookmarkMargin* _margin )
        : QObject( _margin ),
            margin( _margin ),
			bookmarkgroups(), bmgroup_first(-1)
    {
        Q_ASSERT( margin );
        
        connect( margin, SIGNAL( mouseClicked( int, Qt::MouseButton, Qt::MouseButtons, Qt::KeyboardModifiers ) ), this, SLOT( mouseClicked( int, Qt::MouseButton, Qt::MouseButtons, Qt::KeyboardModifiers ) ) );
    }

	void clearBookmarkGroups()
	{
		bookmarkgroups.clear();
	}

	void addBookmarkGroup(int bmgroup)
	{
		bookmarkgroups.insert(bmgroup);
	}


public slots:
    void mouseClicked( int line, Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers ) {
        Q_UNUSED( buttons );
        
        if ( button != Qt::LeftButton || modifiers != Qt::NoModifier ) {
            return;
        }
        
		if (bmgroup_first != -1) {
			margin->editor()->toggleBookmark(bmgroup_first, line);
		}
    }
};

// LineBookmarkMargin

LineBookmarkMargin::LineBookmarkMargin( MarginStacker* marginStacker )
    : AbstractMargin( marginStacker ),
        d( new LineBookmarkMarginPrivate( this ) )
{
    updateWidthRequested();
}

LineBookmarkMargin::~LineBookmarkMargin()
{
}

Global::Margin LineBookmarkMargin::margin_type() const
{
	return Global::Margin::BookmarkMargin;
}

void LineBookmarkMargin::clearBookmarkGroups()
{
	d->clearBookmarkGroups();
}

void LineBookmarkMargin::addBookmarkGroup(int bmgroup)
{
	d->addBookmarkGroup(bmgroup);
}


void LineBookmarkMargin::paintEvent( QPaintEvent* event )
{
    AbstractMargin::paintEvent( event );
    
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing, false );
    painter.setRenderHint( QPainter::SmoothPixmapTransform, true );
    
    const int firstLine = firstVisibleLine( event->rect() );
    const int lastLine = lastVisibleLine( event->rect() );
    const TextDocument* document = editor()->textDocument();
    const QString iconKey = "bookmarks";
    
    for ( QTextBlock block = document->findBlockByNumber( firstLine ); block.isValid() && block.blockNumber() <= lastLine; block = block.next() ) {
        const QRect rect = blockRect( block ).adjusted( LineBookmarkMarginMargins, 0, -( LineBookmarkMarginMargins +1 ), 0 ); // +1 for the 1pixel border
        const TextBlockUserData* data = document->testUserData( block );
        
        if ( data && data->bookmarkgroups.size() > 0 ) {
			int bmgroup;
			foreach(bmgroup, data->bookmarkgroups) {
				if (d->bookmarkgroups.empty() || d->bookmarkgroups.contains(bmgroup)) {
					BookmarkGroup *bookmarkgroup = stacker()->editor()->bookmarkGroups().getGroup(bmgroup);
					if (bookmarkgroup) {
						const int size = qMin(rect.width(), rect.height());
						const QSize pixmapSize(size, size);
						QPixmap pixmap = bookmarkgroup->icon().pixmap(pixmapSize);

						QRect pixmapRect(QPoint(), pixmapSize);
						pixmapRect.moveCenter(rect.center());

						painter.drawPixmap(pixmapRect, pixmap);
					}
				}
			}
        }
    }
    
    if ( event->rect().right() < rect().right() ) {
        return;
    }
    
    painter.setPen( QPen( QColor( palette().color( backgroundRole() ).darker() ), 1 ) );
    painter.drawLine( event->rect().topRight(), event->rect().bottomRight() );
}

void LineBookmarkMargin::updateWidthRequested()
{
    setMinimumWidth( 10 +( LineBookmarkMarginMargins *2 ) +1 ); // +1 for the 1 pixel border
}

} }

#include "LineBookmarkMargin.moc"