#pragma once

#include <Qt3D/QGLView.h>
#include <Qt3D/QGLBuilder.h>
#include <Qt3D/QGLCube.h>
#include <Qt3D/QGLSphere.h>
#include <Qt3D/QGLCylinder.h>
#include <Qt3D/QGLTeapot.h>
#include <QTimer>
#include <QKeyEvent>

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
		type = 0;

		//
		QGLBuilder builder;
        builder << QGL::Faceted;
        builder << QGLCube();
        cube = builder.finalizedSceneNode();

		//
		QGLBuilder builder2;
        builder2 << QGL::Faceted;
        builder2 << QGLSphere();
        sphere = builder2.finalizedSceneNode();

		//
		QGLBuilder builder3;
		builder3 << QGL::Faceted;
        builder3 << QGLCylinder();
        cylinder = builder3.finalizedSceneNode();
		
		//
		QGLBuilder builder4;
		builder4 << QGL::Faceted;
        builder4 << QGLTeapot();
        teapot = builder4.finalizedSceneNode();


		// 定期的に画面を更新
        QTimer *timer = new QTimer( this );
        connect( timer, SIGNAL( timeout() ), this, SLOT( updateGL() ) );
        timer->setInterval( 16 );
        timer->setSingleShot( false );
        timer->start();
	}

	void paintGL(QGLPainter *painter)
	{
		if( type == 0 )
		{
			cube->draw(painter);
		}
		else if( type == 1 )
		{
			sphere->draw(painter);
		}
		else if( type == 2 )
		{
			cylinder->draw(painter);
		}
		else
		{
			teapot->draw(painter);
		}
	}

private:
	QGLSceneNode *cube;
	QGLSceneNode *sphere;
	QGLSceneNode *cylinder;
	QGLSceneNode *teapot;

	int type;

	void keyPressEvent( QKeyEvent *key )
    {
		if( key->key() == Qt::Key_A)
		{
			type = 0;
		}		
		else if( key->key() == Qt::Key_S)
		{
			type = 1;
		}
		else if( key->key() == Qt::Key_D)
		{
			type = 2;
		}
		else if( key->key() == Qt::Key_F)
		{
			type = 3;
		}
	 }
};