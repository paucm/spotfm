/*
   Copyright 2009 Last.fm Ltd. 
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   ella is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   liblastfm is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with liblastfm.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ELLA_XMLQUERY_H
#define ELLA_XMLQUERY_H

#include <QDomDocument>
#include <QDomElement>

namespace ella
{
    class XmlQuery
    {
        QDomDocument domdoc;
        QDomElement e;

    public:
        XmlQuery( const QByteArray& );
        
        XmlQuery( const QDomElement& e, const char* name = "" ) : e( e )
        {
            if (e.isNull()) qDebug("Expected node absent: %s", name);
        }

        XmlQuery operator[]( const QString& name ) const;
        QString text() const { return e.text(); }
        QString attribute(const QString &name) const { return e.attribute(name); }
        
        QList<XmlQuery> children( const QString& named ) const;

        operator QDomElement() const { return e; }
    };
}

#endif
