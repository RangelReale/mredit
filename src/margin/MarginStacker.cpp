#include "mredit/margin/MarginStacker.h"
#include "mredit/Editor.h"
#include "mredit/margin/AbstractMargin.h"
#include "mredit/margin/LineBookmarkMargin.h"
#include "mredit/margin/LineNumberMargin.h"
#include "mredit/margin/LineRevisionMargin.h"
#include "mredit/margin/LineSpacingMargin.h"

#include <QHBoxLayout>
#include <QMap>
#include <QTimer>
#include <QDebug>

namespace mredit {
namespace margin {

// MarginStackerPrivate

class MarginStackerPrivate : public QObject {
    Q_OBJECT

public:
    //QMap<Global::Margin, AbstractMargin*> margins;
	QMap<int, AbstractMargin*> margins;
	int next_margin_id;
    MarginStacker* stacker;
    QHBoxLayout* layout;
    Editor* editor;
    QTimer* updateLayoutTimer;
    
    MarginStackerPrivate( MarginStacker* _stacker )
        : QObject( _stacker ),
			next_margin_id(1),
            stacker( _stacker ),
            layout( new QHBoxLayout( stacker ) ),
            editor( 0 ),
            updateLayoutTimer( new QTimer( this ) )
    {
        Q_ASSERT( stacker );
        
        layout->setMargin( 0 );
        layout->setSpacing( 0 );
        
        updateLayoutTimer->setInterval( 20 );
        updateLayoutTimer->setSingleShot( true );
        
        connect( updateLayoutTimer, SIGNAL( timeout() ), this, SLOT( updateLayoutTimeout() ) );
    }

	QList<AbstractMargin*> getMargins(Global::Margin margin_type) {
		QList<AbstractMargin*> ret;
		for (auto *margin : margins) {
			if (margin->margin_type() == margin_type) {
				ret.append(margin);
			}
		}
		return ret;
	}

	bool isVisible(int margin_id) {
		AbstractMargin* margin = margins.value(margin_id);

		if (!margin) {
			return false;
		}

		return margin->isVisible();
	}

	bool isVisible(Global::Margin margin_type) {
		auto list = getMargins(margin_type);
		if (list.size() > 0) {
			return list[0]->isVisible();
		}
		return false;
	}

	void setVisible(int margin_id, bool visible) {
		AbstractMargin* margin = margins.value(margin_id);

		if (!margin) {
			return;
		}

		margin->setVisible(visible);
	}

	int setVisible(Global::Margin margin_type, bool visible, bool addIfNotExists) {
		int found = -1;
		foreach(AbstractMargin *m, getMargins(margin_type)) {
			found = m->marginId();
			setVisible(m->marginId(), visible);
		}
		if (found == -1 && addIfNotExists) {
			found = addMargin(margin_type, true);
		}
		return found;
	}

	int addMargin(AbstractMargin *margin)
	{
		if (!margin) {
			//Q_ASSERT( margin );
			return -1;
		}
		int cur_id = next_margin_id++;
		margins[cur_id] = margin;

		connect(margin, SIGNAL(resized()), stacker, SLOT(updateLayout()));

		layout->addWidget(margin);
		margin->setMarginId(cur_id);
		margin->setVisible(true);
		margin->setEditor(editor);

		return cur_id;
	}

	int addMargin(Global::Margin margin_type, bool onlyIfNotExists)
	{
		if (onlyIfNotExists) {
			auto list = getMargins(margin_type);
			if (list.size() > 0) {
				return list[0]->marginId();
			}
		}

		AbstractMargin *margin = nullptr;
		switch (margin_type) {
		case Global::Margin::NumberMargin:
			margin = new LineNumberMargin(stacker);
			break;
		case Global::Margin::FoldMargin:
			//margin = new LineFoldMargin( stacker );
			break;
		case Global::Margin::BookmarkMargin:
			margin = new LineBookmarkMargin(stacker);
			break;
		case Global::Margin::RevisionMargin:
			margin = new LineRevisionMargin(stacker);
			break;
		case Global::Margin::SpaceMargin:
			margin = new LineSpacingMargin(stacker);
			break;
		default:
			Q_ASSERT(0);
			break;
		}

		if (!margin) {
			//Q_ASSERT( margin );
			return -1;
		}

		return addMargin(margin);
	}

public slots:
    void updateLayout() {
        updateLayoutTimer->start();
    }
    
    void updateLayoutTimeout() {
        if ( !editor ) {
            return;
        }
        
        const int margin = editor->frameWidth();
        int width = 0;
        
        foreach ( AbstractMargin* margin, margins.values() ) {
            width += margin->minimumWidth();
        }
        
        editor->setViewportMargins( width, 0, 0, 0 );
        stacker->setGeometry( QRect( QPoint( margin, margin ), QSize( width, editor->viewport()->height() ) ) );
    }
};

// MarginStacker

MarginStacker::MarginStacker( Editor* editor )
    : QWidget( editor ),
        d( new MarginStackerPrivate( this ) )
{
    setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Expanding );
    
    if ( editor ) {
        editor->setMarginStacker( this );
    }
}

MarginStacker::~MarginStacker()
{
}

AbstractMargin* MarginStacker::margin(int margin_id) const
{
	return d->margins.value(margin_id);
}

AbstractMargin* MarginStacker::margin(Global::Margin type ) const
{
	auto tmp = d->getMargins(type);
	if (tmp.size() == 0) return nullptr;
	return tmp[0];
}

QList<AbstractMargin*> MarginStacker::margins(Global::Margin margin_type)
{
	return d->getMargins(margin_type);
}

int MarginStacker::addMargin(AbstractMargin* margin)
{
	return d->addMargin(margin);
}

int MarginStacker::addMargin(Global::Margin margin_type)
{
	return d->addMargin(margin_type, true);
}

Editor* MarginStacker::editor() const
{
    return d->editor;
}

void MarginStacker::setEditor( Editor* editor )
{
    if ( d->editor ) {
        d->editor->removeEventFilter( this );
    }
    
    d->editor = editor;
    
    foreach ( AbstractMargin* margin, d->margins ) {
        margin->setEditor( editor );
    }
    
    if ( d->editor ) {
        d->editor->installEventFilter( this );
        setParent( d->editor );
        QWidget::setVisible( true );
        updateLayout();
    }
}

bool MarginStacker::isVisible(int margin_id) const
{
	//return d->margins.value(margin_id) != 0;
	return d->isVisible(margin_id);
}

bool MarginStacker::isVisible(Global::Margin type ) const
{
	return d->isVisible(type);
}

void MarginStacker::setVisible(int margin_id, bool visible)
{
	d->setVisible(margin_id, visible);
}

int MarginStacker::setVisible(Global::Margin type, bool visible, bool addIfNotExists)
{
    return d->setVisible( type, visible, addIfNotExists);
}

void MarginStacker::updateLayout()
{
    d->updateLayout();
}

bool MarginStacker::eventFilter( QObject* object, QEvent* event )
{
    if ( object == d->editor ) {
        if ( event->type() == QEvent::Resize ) {
            d->updateLayout();
            return true;
        }
    }
    
    return QWidget::eventFilter( object, event );
}

} }

#include "MarginStacker.moc"
