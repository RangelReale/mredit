#pragma once


#include <QFrame>
#include <QTextDocument>
#include <QPaintEvent>
#include <QResizeEvent>

namespace mredit {

class LabelPrivate;

class Label : public QFrame
{
    Q_OBJECT
public:
	Label(QWidget* parent = 0);
	Label(const QString &text, QWidget* parent = 0);
	virtual ~Label();

	void setDocument(QTextDocument *document);
	QTextDocument *document() const;

    inline QString toPlainText() const
    { return document()->toPlainText(); }

	QSize sizeHint() const override;
	QSize minimumSizeHint() const override;
public Q_SLOTS:
	void setPlainText(const QString &text);
	void clear();
protected:
	void resizeEvent(QResizeEvent *event) override;
	void paintEvent(QPaintEvent *event) override;
private:
	LabelPrivate *d;
};

}
