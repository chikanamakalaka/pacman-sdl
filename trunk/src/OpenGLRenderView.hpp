/*
 * OpenGLRenderView.hpp
 *
 *  Created on: May 4, 2009
 *      Author: asantos
 */
#undef CreateEvent

#include <GL/gl.h>
#include "TextureFactory.hpp"


class OpenGLRenderView{
public:
	typedef int int_tp;
	typedef void(Initialized)();
private:
	SignalBroker& signalbroker;
	boost::signals::connection sdlinitializedconnection;
	bool initialized;
	bool openglscenenodeprocessorregistered;

	OpenGL::DisplayListFactory displaylistfactory;
	OpenGL::VBOFactory vbofactory;
	TextureFactory<OpenGL::Texture> texturefactory;
public:
	OpenGLRenderView(SignalBroker& signalbroker):
		signalbroker(signalbroker),
		initialized(false),
		openglscenenodeprocessorregistered(false),
		texturefactory(signalbroker){

		sdlinitializedconnection = signalbroker.ConnectToSignal
		<SDLRenderView::Initialized>
		(	"/render/sdlinitialized",
			boost::bind(&OpenGLRenderView::Initialize, this));

		signalbroker.ConnectToSignal
		<SceneGraphController::GetSelectedSceneGraphHandler>
		(	"/scenegraphcontroller/getselectedscenegraph",
			boost::bind(&OpenGLRenderView::RenderAll, this, _1));

		signalbroker.ConnectToSignal
		<SceneGraphController::GetSelectedSceneGraphHandler>
		(	"/openglrenderview/beginrender",
			boost::bind(&OpenGLRenderView::BeginRender, this, _1));

		signalbroker.ConnectToSignal
		<SceneGraphController::GetSelectedSceneGraphHandler>
		(	"/openglrenderview/render",
			boost::bind(&OpenGLRenderView::Render, this, _1));

		signalbroker.ConnectToSignal
		<SceneGraphController::GetSelectedSceneGraphHandler>
		(	"/openglrenderview/endrender",
			boost::bind(&OpenGLRenderView::EndRender, this, _1));

	}
protected:
	void Initialize(){
		try{
			Signal<Initialized>& openglinitialized = signalbroker.EnsureSignal<Initialized>("/render/openglinitialized");

			/*bool err = GLeeInit();
			if(err){
				signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", std::string("Error initializing GL extension wrangler (GLEW)")+GLeeGetErrorString());
			}else{
				signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Success initializing GL extension wrangler (GLEW)");
			}*/

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Enabling 2D textures.");
			glEnable( GL_TEXTURE_2D );

			glEnable(GL_VERTEX_ARRAY);
			glEnable(GL_TEXTURE_COORD_ARRAY);

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Changing clear color.");
			glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Setting up viewport.");
			glViewport( 0, 0, 640, 480 );

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Clearing color buffer.");
			glClear( GL_COLOR_BUFFER_BIT );

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Initializing projection matrix.");
			glMatrixMode( GL_PROJECTION );
			glLoadIdentity();

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Initializing orthogonal mode.");
			glOrtho(0.0f, 640, 480, 0.0f, -1000.0f, 1000.0f);

			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Initializing model view matrix.");
			glMatrixMode( GL_MODELVIEW );
			glLoadIdentity();

			//disconnect from sdlinitialize when done. don't want to initialize more than once
			sdlinitializedconnection.disconnect();
			initialized = true;
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "OpenGL initialized.");

			openglinitialized();
		}catch(SignalDoesNotExist& e){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Caught EventDoesNotExist");
		}
	}
	void RenderAll(boost::shared_ptr<SceneGraph> scenegraph){
		if(initialized){
			//signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Rendering scenegraph with OpenGL");
			signalbroker.InvokeSignal<SceneGraphController::GetSelectedSceneGraphHandler>
			("/openglrenderview/beginrender", scenegraph);

			signalbroker.InvokeSignal<SceneGraphController::GetSelectedSceneGraphHandler>
			("/openglrenderview/render", scenegraph);

			signalbroker.InvokeSignal<SceneGraphController::GetSelectedSceneGraphHandler>
			("/openglrenderview/endrender", scenegraph);
		}
	}
	void BeginRender(boost::shared_ptr<SceneGraph> scenegraph){
		glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT );
	}
	void Render(boost::shared_ptr<SceneGraph> scenegraph){
		//if opengl scene node processor is not registered, register it
		if(!scenegraph->HasNodeProcessor("geometry")){
			std::list<std::string> geometrydependencies;
			scenegraph->RegisterNodeProcessor("geometry", geometrydependencies, boost::bind(&OpenGLRenderView::OpenGLNodeProcessor, this, _1));
			scenegraph->VisitNodes(boost::bind(&OpenGLRenderView::OpenGLNodeProcessor, this, _1));
		}

		//render the scenegraph
		//signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Rendering SceneGraph with OpenGL.");
		RenderSceneNodeAndChildren(scenegraph->GetRoot());
	}
	void EndRender(boost::shared_ptr<SceneGraph> scenegraph){
		SDL_GL_SwapBuffers();
	}

	void OpenGLNodeProcessor(SceneGraph::SceneNode& scenenode){
		//has geometry but no openglvbo?
		if(scenenode.HasSceneNodeProperty("geometry") && !scenenode.HasSceneNodeProperty("renderable") && scenenode.HasSceneNodeProperty("texture")){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "Processing geometry property into opengldisplaylist property.");
			//create the openglvbo from the geometry
			Geometry& geometry = scenenode.GetSceneNodeProperty<GeometryProperty>("geometry").GetGeometry();
			//PositionProperty& positionproperty scenenode.GetSceneNodeProperty<PositionProperty>("position");
			const TriangleStrip& trianglestrip = dynamic_cast<const TriangleStrip&>(geometry);


			boost::shared_ptr<ITexture> texture = texturefactory.LoadFromFilePath(scenenode.GetSceneNodeProperty<TextureProperty>("texture").GetPathToTexture());

			if(true){
				boost::shared_ptr<IRenderable> vbo = vbofactory.CreateFromTriangleStrip(trianglestrip);
				scenenode.AddSceneNodeProperty("renderable", boost::shared_ptr<SceneNodeProperty>(new RenderableProperty(vbo, texture)));

			}else{
			//boost::shared_ptr<IRenderable> displaylist = displaylistfactory.CreateFromTriangleStrip(trianglestrip);
			//scenenode.AddSceneNodeProperty("renderable", boost::shared_ptr<SceneNodeProperty>(new RenderableProperty(displaylist, texture)));
			}
		}

	}
	void RenderSceneNodeAndChildren(SceneNode& scenenode){
		if(scenenode.HasSceneNodeProperty("position")){
			const PositionProperty& positionproperty = scenenode.GetSceneNodeProperty<PositionProperty>("position");
			PushMatrix4(positionproperty.GetPosition());
		}
		RenderSceneNode(scenenode);
		const SceneNode::SceneNodes& children = scenenode.GetChildNodes();
		SceneNode::SceneNodes::const_iterator itr = children.begin();
		for(;itr!=children.end(); itr++){
			RenderSceneNodeAndChildren(**itr);
		}
		if(scenenode.HasSceneNodeProperty("position")){
			glPopMatrix();
			//signalbroker.InvokeSignal("/log/output", (ArgsList(),"Popping matrix"));
		}
	}
	void RenderSceneNode(SceneNode& scenenode){
		/*if(scenenode.HasSceneNodeProperty("openglvbo")){
			scenenode.GetSceneNodeProperty<RenderableProperty>("openglvbo").Render();
		}*/
		try{
			if(scenenode.HasSceneNodeProperty("renderable")){
				//signalbroker.InvokeSignal("/log/output", (ArgsList(),"Rendering SceneNode with OpenGL."));

				const RenderableProperty& renderableproperty = scenenode.GetSceneNodeProperty<RenderableProperty>("renderable");
				renderableproperty.BindTexture();
				renderableproperty.Render();
			}
		}catch(SceneNodePropertyDoesNotExist e){
			signalbroker.InvokeSignal<OutputStreamView::LogHandler>("/log/output", "SceneNode does not have renderable property.");
		}
	}
private:
	void PushMatrix4(const Matrix4& m){
		GLfloat* mptr = new GLfloat[16];
		for(int i=0; i<4; i++){
			for(int j=0; j<4; j++){
				mptr[j*4+i] = m(i,j);
			}
		}
		glPushMatrix();
		glMultMatrixf(mptr);
		delete mptr;
	}
};
