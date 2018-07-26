#pragma once

#include <QWidget>

#include "mredit/Global.h"

namespace mredit {

class Editor;

namespace margin {

class MarginStackerPrivate;
class AbstractMargin;

class MarginStacker : public QWidget
{
    Q_OBJECT
    friend class MarginStackerPrivate;
    
public:
    MarginStacker( Editor* editor = 0 );
    virtual ~MarginStacker();
    
	AbstractMargin* margin(int margin_id) const;
	AbstractMargin* margin( Global::Margin type ) const;
	QList<AbstractMargin*> margins(Global::Margin margin_type);
	int addMargin(AbstractMargin* margin);
	int addMargin(Global::Margin margin_type);

    Editor* editor() const;
    void setEditor( Editor* editor );
    
	bool isVisible(int margin_id) const;
	bool isVisible( Global::Margin type ) const;
	void setVisible(int margin_id, bool visible = true);
	int setVisible( Global::Margin type, bool visible = true, bool addIfNotExists = true );
    
public slots:
    void updateLayout();

protected:
    virtual bool eventFilter( QObject* object, QEvent* event );

private:
    MarginStackerPrivate* d;
};

} }
