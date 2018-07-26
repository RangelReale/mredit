#pragma once

#include <QObject>
#include <QIcon>
#include <QMap>
#include <QTextBlockFormat>

namespace mredit {

class BookmarkGroupPrivate;
class BookmarkGroupListPrivate;
    
class BookmarkGroup : public QObject
{
	Q_OBJECT
	friend class BookmarkGroupPrivate;
public:
	BookmarkGroup(const QString &title, const QIcon &icon, QObject *parent = 0);
	virtual ~BookmarkGroup();

	const QString &title() const;
	void setTitle(const QString &title);

	const QIcon &icon() const;
	void setIcon(const QIcon &icon);

	QTextBlockFormat format() const;
	void setTextBlockFormat(QTextBlockFormat format);
private:
	BookmarkGroupPrivate * d;
};

class BookmarkGroupList : public QObject
{
	Q_OBJECT
	friend class BookmarkGroupListPrivate;
public:
	BookmarkGroupList(QObject *parent = 0);
	virtual ~BookmarkGroupList();

	int addGroup(const QString &title, const QIcon &icon);
	BookmarkGroup *getGroup(int bmid) const;
private:
	BookmarkGroupListPrivate * d;
};

}
