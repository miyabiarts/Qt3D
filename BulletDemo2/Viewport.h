#pragma once

#include <Qt3D/QGLView.h>
#include <Qt3D/QGLBuilder.h>
#include <Qt3D/QGLCube.h>
#include <QTimer>
#include <QKeyEvent>

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

class Viewport : public QGLView
{
Q_OBJECT
public:
	Viewport() :
	  QGLView(0)
	{
	}

	~Viewport()
	{
		release();
	}

	void initializeGL(QGLPainter * painter)
    {
		QGLBuilder builder;
        builder << QGL::Faceted;
        builder << QGLCube();

        cube = builder.finalizedSceneNode();

		angle = 0.0f;

		logo.setImage(QImage("mami.jpg"));

        camera()->setEye(QVector3D(0, 0, 50));
        camera()->setCenter(QVector3D(0.0f,2.0f,5.0f));
        camera()->setFarPlane(1000);

		initializeWorld();

		// ����I�ɉ�ʂ��X�V
        QTimer *timer = new QTimer( this );
        connect( timer, SIGNAL( timeout() ), this, SLOT( updateGL() ) );
        timer->setInterval( 16 );
        timer->setSingleShot( false );
        timer->start();
	}

	

	void paintGL(QGLPainter *painter)
	{
        world_->stepSimulation( 1.0f / 60.0f, 1 );

        btScalar m[ 16 ];
        QMatrix4x4 m2;

        //�I�u�W�F�N�g�̐����擾
        int numObject = world_->getNumCollisionObjects();

        for( int i = 0; i < numObject; ++i )
        {
            btCollisionObject* colObj = world_->getCollisionObjectArray()[ i ];
            btRigidBody* body = btRigidBody::upcast( colObj );
            body->activate();

            if( body && body->getMotionState() )
            {
                btDefaultMotionState* motion = static_cast< btDefaultMotionState* >( body->getMotionState() );
                motion->m_graphicsWorldTrans.getOpenGLMatrix( m );
            }
            else
            {
                colObj->getWorldTransform().getOpenGLMatrix( m );
            }

			if( colObj->getCollisionShape()->getShapeType() == STATIC_PLANE_PROXYTYPE)
			{
		        painter->setStandardEffect(QGL::FlatColor);
				painter->setColor(QColor(127, 127, 127, 255));

				QVector3DArray vertices;
				vertices.append(-50.0f,  0.0f, -50.0f);
				vertices.append( 50.0f,  0.0f, -50.0f);
				vertices.append( 50.0f,  0.0f,  50.0f);
				vertices.append(-50.0f,  0.0f,  50.0f);

				painter->clearAttributes();
				painter->setVertexAttribute(QGL::Position, vertices);
				painter->draw(QGL::TriangleFan, 4);
			}
            else if( colObj->getCollisionShape()->getShapeType() == BOX_SHAPE_PROXYTYPE )
            {
                for( int j = 0 ; j < 16 ; ++j) m2(j%4,j/4) = m[j];
                painter->modelViewMatrix().push();
                painter->modelViewMatrix() *= m2;
		        painter->setStandardEffect(QGL::LitDecalTexture2D);
			    painter->setFaceColor(QGL::AllFaces, QColor(170, 202, 0));
     			logo.bind();
                cube->draw(painter);
                painter->modelViewMatrix().pop();
            }
        }


	}

private:
	QGLSceneNode *cube;
	float angle;

	QGLTexture2D logo;

	btDynamicsWorld *world_;                                      // �������E
    btDefaultCollisionConfiguration *collisionConfiguration_;     // �������Ǘ�
    btCollisionDispatcher *dispatcher_;                           // �����x�Փˌ��m
    btBroadphaseInterface *broadphase_;                           // �ᐸ�x�Փˌ��m
    btConstraintSolver *solver_;                                  // �Փˏ���
    btAlignedObjectArray< btCollisionShape* > collisionShapes_;   // �Փ˕��̃��X�g

	void initializeWorld()
	{
		//�������E���쐬
        collisionConfiguration_ = new btDefaultCollisionConfiguration();

        dispatcher_ = new btCollisionDispatcher( collisionConfiguration_ );

        float maxl = 10000;
        btVector3 worldAabbMin( -maxl, -maxl, -maxl );
        btVector3 worldAabbMax(  maxl,  maxl,  maxl );
        broadphase_ = new btAxisSweep3( worldAabbMin, worldAabbMax, 5 * 5 * 5 + 1024 );

        solver_ = new btSequentialImpulseConstraintSolver();

        world_ = new btDiscreteDynamicsWorld( dispatcher_, broadphase_, solver_, collisionConfiguration_ );

        //�d�͂̐ݒ�
        world_->setGravity( btVector3( 0, -9.81, 0 ) );

		reset();
	}

		void release()
	{
		//
		clearObjects();

		//�������E�̍폜
		delete world_; 
		delete solver_; 
		delete broadphase_; 
		delete dispatcher_;
		delete collisionConfiguration_;
	}

	//
	void clearObjects()
	{

		for( int i = world_->getNumCollisionObjects() - 1 ; i >= 0  ; --i )
		{
			btCollisionObject *obj =  world_->getCollisionObjectArray()[ i ];

			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState())
			{
				delete body->getMotionState();
			}
			world_->removeCollisionObject( obj );
			delete obj;
		}
		
		for( int i = 0 ; i < collisionShapes_.size() ; ++i )
		{
			delete collisionShapes_[ i ];
		}
		collisionShapes_.clear();		
	}

	void reset()
	{
		clearObjects();
		
        addGround();
		addBox(btVector3(0.0f,0.5f,0.0f));
	}

	btRigidBody *addGround()
    {
        btCollisionShape *shape = new btStaticPlaneShape( btVector3( 0, 1, 0 ), 0 );
        collisionShapes_.push_back( shape );

        btTransform transform;
        transform.setIdentity();

        btDefaultMotionState* motionState = new btDefaultMotionState( transform );
        btRigidBody::btRigidBodyConstructionInfo rbInfo( 0.0f, motionState, shape, btVector3( 0.0f, 0.0f, 0.0f ) );
        btRigidBody *body = new btRigidBody( rbInfo );
        world_->addRigidBody( body );

        return body;
    }

    //
    btRigidBody *addBox( const btVector3 &position, const btVector3 &size = btVector3( 0.5f, 0.5f, 0.5f ), float mass = 1.0f )
    {
        //�`��̐���
        btCollisionShape *shape = new btBoxShape( size );
        collisionShapes_.push_back( shape );

        //�����ʒu�̐ݒ�
        btMatrix3x3 mat;
        mat.setIdentity();
        btDefaultMotionState *motion = new btDefaultMotionState( btTransform( mat, position ) );


        //���ʂ��犵���̌v�Z
        btVector3 localInertia;
        shape->calculateLocalInertia( mass, localInertia );

        btRigidBody::btRigidBodyConstructionInfo info( mass, motion, shape, localInertia );

        //���̂̐���
        btRigidBody *body( new btRigidBody( info ) );


        //�o�^
        world_->addRigidBody( body );

        return body;
    }

	 void keyPressEvent( QKeyEvent *key )
    {
		float power = 0.0f;

		if( key->key() == Qt::Key_A)
		{
			reset();
		}		
		else if( key->key() == Qt::Key_S)
		{
			power = 10.0f;
		}
		else if( key->key() == Qt::Key_D)
		{
			power = 100.0f;
		}
		else if( key->key() == Qt::Key_F)
		{
			power = 1000.0f;
		}
		else if( key->key() == Qt::Key_G)
		{
			power = 10000.0f;
		}
		else if( key->key() == Qt::Key_H)
		{
			power = 100000.0f;
		}
		else if( key->key() == Qt::Key_J)
		{
			power = 530000.0f;
		}
		else
		{
	        QGLView::keyPressEvent(key);

		}

		if( power != 0.0f )
		{
			btCollisionObject* colObj = world_->getCollisionObjectArray()[ 1 ];
            btRigidBody* body = btRigidBody::upcast( colObj );
            body->activate();
			body->applyForce(btVector3(0.0f, 0.0f, -power), btVector3( 0.0f, -0.4f, 0.0f ) );

		}
    }
};