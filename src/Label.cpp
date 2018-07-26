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
	QTextDocument *doc;

	LabelPrivate(Label* widget) :
		widget(widget), doc(nullptr)
	{
		Q_ASSERT(widget);

		doc = new QTextDocument(widget);
	}
};

// Public

Label::Label(QWidget* parent) :
	QFrame(parent),
	d(new LabelPrivate(this))
{
}

Label::Label(const QString &text, QWidget* parent) :
	Label(parent)
{
	setPlainText(text);
}

Label::~Label()
{
	delete d;
}

void Label::setDocument(QTextDocument *document)
{
	if (!document) {
		document = new QTextDocument(this);
		//documentLayout = new QPlainTextDocumentLayout(document);
		//document->setDocumentLayout(documentLayout);
	}
	else {
		/*
		documentLayout = qobject_cast<QPlainTextDocumentLayout*>(document->documentLayout());
		if (Q_UNLIKELY(!documentLayout)) {
			qWarning("QPlainTextEdit::setDocument: Document set does not support QPlainTextDocumentLayout");
			return;
		}
		*/
	}
	if (d->doc) {
		delete d->doc;
	}

	d->doc = document;
}

QTextDocument *Label::document() const
{
	return d->doc;
}

void Label::setPlainText(const QString &text)
{
	d->doc->setPlainText(text);
	update();
}

void Label::clear()
{
	d->doc->clear();
}

QSize Label::sizeHint() const
{
	return d->doc->documentLayout()->documentSize().toSize();
	/*
	QSize ret = QFrame::sizeHint();
	qDebug() << d->doc->documentLayout()->documentSize().toSize();
	return ret;
	*/
}

QSize Label::minimumSizeHint() const
{
	QSize ret = QFrame::minimumSizeHint();
	return ret;
}

void Label::resizeEvent(QResizeEvent *event)
{
	QFrame::resizeEvent(event);
	d->doc->setTextWidth(event->size().width());
}

void Label::paintEvent(QPaintEvent *event)
{
	QFrame::paintEvent(event);
	QPainter painter(this);
	d->doc->drawContents(&painter, rect());
}

}
