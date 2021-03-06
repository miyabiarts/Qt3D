#pragma once

#include <Qt3D/QGLView.h>
#include <QTimer>

class Viewport : public QGLView
{
Q_OBJECT
public:
	Viewport() :
	  QGLView(0)
	{
	}

	void initializeGL(QGLPainter * painter)
    {
		// 定期的に画面を更新
        QTimer *timer = new QTimer( this );
        connect( timer, SIGNAL( timeout() ), this, SLOT( updateGL() ) );
        timer->setInterval( 16 );
        timer->setSingleShot( false );
        timer->start();
	}

	void paintGL(QGLPainter *painter)
	{
	}

private:
};