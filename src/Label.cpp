#include "mredit/Label.h"

#include <QPainter>
#include <QDebug>
#include <QAbstractTextDocumentLayout>

// http://www.java2s.com/Code/Cpp/Qt/DrawQTextDocumentwithQTextDocument.htm

namespace mredit {

// Private
class LabelPrivate
{
public:
	Label *widget;
	QTextDocument *_doc;

	LabelPrivate(Label* widget) :
		widget(widget), _doc(nullptr)
	{
		Q_ASSERT(widget);

	}

	QTextDocument *doc()
	{
		if (!_doc) {
			_doc = new QTextDocument(widget);
			_doc->setDefaultFont(widget->font());
		}
		return _doc;
	}

	void setDoc(QTextDocument *docu)
	{
		if (_doc)
			delete _doc;
		_doc = docu;
		if (_doc)
			_doc->setDefaultFont(widget->font());
	}
};

// Public

Label::Label(QWidget* parent) :
	QFrame(parent),
	d(new LabelPrivate(this))
{
}

Label::~Label()
{
	delete d;
}

void Label::setDocument(QTextDocument *document)
{
	d->setDoc(document);
}

QTextDocument *Label::document() const
{
	return d->doc();
}

void Label::setPlainText(const QString &text)
{
	d->doc()->setPlainText(text);
	update();
}

void Label::clear()
{
	d->doc()->clear();
}

QSize Label::sizeHint() const
{
	return d->doc()->documentLayout()->documentSize().toSize();
}

QSize Label::minimumSizeHint() const
{
	return QFrame::minimumSizeHint();
}

void Label::resizeEvent(QResizeEvent *event)
{
	QFrame::resizeEvent(event);
	d->doc()->setTextWidth(event->size().width());
}

void Label::paintEvent(QPaintEvent *event)
{
	QFrame::paintEvent(event);
	QPainter painter(this);
	d->doc()->drawContents(&painter, rect());
}

}
