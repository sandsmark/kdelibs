#ifndef MATIC_H
#define MATIC_H

#include <qstring.h>
#include <qlist.h>

/*static QString prefix = "";*/

/* forward definitions */
void initMaticFlex(const char*);
void initMaticParser(const char*);
void maticerror(const char*);
int maticlex();
int maticparse();

class MHash
{
public:
	enum Type { String, Hash };

	MHash(QString* name, QString* value);
	MHash(QString* name, int value);
	MHash(QString* name, MHash *hash);
	~MHash();

	void addHash(MHash *hash);
	/*void dump();*/

	QString	*name;
	QString	*value;
	QList<MHash>	hashes;
	Type	type;
};

inline MHash::MHash(QString* _name, QString* _value)
{ type = MHash::String; name = _name; value = _value; hashes.setAutoDelete(true); }

inline MHash::MHash(QString *_name, int val)
{ type = MHash::String; name = _name; value = new QString; value->setNum(val); hashes.setAutoDelete(true); }

inline MHash::MHash(QString* _name, MHash *_hash)
{ name = _name; hashes.setAutoDelete(true); value = 0; addHash(_hash); }

inline MHash::~MHash()
{ if (name) delete name; if (value) delete value; }

/*inline void MHash::dump()
{
	switch (type)
	{
		case MHash::String:
			{
				debug("%s%s => %s",prefix.latin1(),(name ? name->latin1() : "<unnamed>"),(value ? value->latin1() : "undef"));
				break;
			}
		case MHash::Hash:
			{
				debug("%s%s => {",prefix.latin1(),(name ? name->latin1() : "<unnamed>"));
				prefix.append("  ");
				QListIterator<MHash>	it(hashes);
				for (;it.current();++it)
					it.current()->dump();
				prefix.truncate(prefix.length()-2);
				debug("%s} (%s)",prefix.latin1(),(name ? name->latin1() : "<unnamed>"));
				break;
			}
	}
}*/

/***********************************************************************************************************/

#include <qmap.h>
#include <qdict.h>

class MaticBlock
{
public:
	MaticBlock();
	void readMHash(MHash*);

	MaticBlock* block(const QString& name)	const	{ return m_blocks.find(name); }
	const QString& arg(const QString& name) const	{ return m_args[name]; }

	QString			m_name;
	QMap<QString,QString>	m_args;
	QDict<MaticBlock>	m_blocks;
};

MaticBlock* loadMaticData(const char *s);

#endif
