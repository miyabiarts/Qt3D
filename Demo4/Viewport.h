#pragma once

#include <Qt3D/QGLView.h>
#include <Qt3D/QGLBuilder.h>
#include <Qt3D/QGLCube.h>
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
		QGLBuilder builder;
        builder << QGL::Faceted;
        builder << QGLCube();

        cube = builder.finalizedSceneNode();

		angle = 0.0f;

		// ’èŠú“I‚É‰æ–Ê‚ðXV
        QTimer *timer = new QTimer( this );
        connect( timer, SIGNAL( timeout() ), this, SLOT( updateGL() ) );
        timer->setInterval( 16 );
        timer->setSingleShot( false );
        timer->start();
	}

	void paintGL(QGLPainter *painter)
	{
		QMatrix4x4 m;
		m.rotate(angle, 1.0f, 1.0f, 0.0f );
		painter->modelViewMatrix() *= m;

        painter->setStandardEffect(QGL::LitMaterial);
        painter->setFaceColor(QGL::AllFaces, QColor(170, 202, 0));
//        painter->setFaceColor(QGL::AllFaces, QColor(255, 0, 0));
	
		cube->draw(painter);

		angle += 1.0f;
	}

private:
	QGLSceneNode *cube;
	float angle;
};