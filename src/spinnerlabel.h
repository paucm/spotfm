/***************************************************************************
 *   Copyright (C) 2005 - 2007 by                                          *
 *      Last.fm Ltd <client@last.fm>                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/

#ifndef SPINNER_LABEL_H
#define SPINNER_LABEL_H

#include <QLabel>
#include <QMovie>

class SpinnerLabel : public QLabel
{
    virtual bool event( QEvent* e )
    {
        switch ((int)e->type())
        {
            case QEvent::Hide:
                m_movie->setPaused( true );
                break;
            case QEvent::Show:
                m_movie->start();
                break;
        }

        return QLabel::event( e );
    }

    QMovie* m_movie;

public:
    SpinnerLabel( QWidget* parent = 0 ) : QLabel( parent )
    {
        m_movie = new QMovie(":/icons/icons/progress.gif");
        setMovie(m_movie);
        m_movie->setParent(this);
    }
};

#endif
