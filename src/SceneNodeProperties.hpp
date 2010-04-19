/*
 * SceneNodeProperties.hpp
 *
 *  Created on: May 6, 2009
 *      Author: asantos
 */

class ITexture:public Cloneable<ITexture>{
public:
	virtual ~ITexture(){}
	virtual void Bind()const{}
	virtual void Unload()const{}
};

class Vertex{
private:
	Vector4 position;
	Vector4 normal;
	Vector3 color;
	Vector2 texturecoords;
public:
	Vertex( float x=0.0f, float y=0.0f, float z=0.0f,
			float tu=0.0f, float tv=0.0f,
			float nx=0.0f, float ny=0.0f, float nz=0.0f)
	{
		position(0) = x;
		position(1) = y;
		position(2) = z;
		position(3) = 1.0f;
		normal(0) = nx;
		normal(1) = ny;
		normal(2) = nz;
		normal(3) = 1.0f;
		color(0) = 1.0f;
		color(1) = 1.0f;
		color(2) = 1.0f;
		texturecoords(0) = tu;
		texturecoords(1) = tv;

	}
	inline Vector4& GetPosition(){
		return position;
	}
	inline const Vector4& GetPosition()const{
		return position;
	}
	inline float GetX()const{
		return position(0);
	}
	inline float GetY()const{
		return position(1);
	}
	inline float GetZ()const{
		return position(2);
	}
	inline Vector4& GetNormal(){
		return normal;
	}
	inline const Vector4& GetNormal()const{
		return normal;
	}
	inline float GetNormalX()const{
		return normal(0);
	}
	inline float GetNormalY()const{
		return normal(1);
	}
	inline float GetNormalZ()const{
		return normal(2);
	}
	inline Vector3& GetColor(){
		return color;
	}
	inline const Vector3& GetColor()const{
		return color;
	}
	inline float GetColorR()const{
		return color(0);
	}
	inline float GetColorG()const{
		return color(1);
	}
	inline float GetColorB()const{
		return color(2);
	}
	inline Vector2& GetTextureCoordinates(){
		return texturecoords;
	}
	inline const Vector2& GetTextureCoordinates()const{
		return texturecoords;
	}
	inline float GetTextureU()const{
		return texturecoords(0);
	}
	inline float GetTextureV()const{
		return texturecoords(1);
	}
};

class Triangle{
private:
	std::list<Vertex> vertices;
public:
	Triangle(){
	}
	const std::list<Vertex>& GetVertices()const{
		return vertices;
	}
	void Clear(){
		vertices.empty();
	}
	void AddVertex(const Vertex& vertex){
		if(vertices.size()<3){
			vertices.push_back(vertex);
		}
	}
};

class Geometry{
private:
	bool visible;
public:
	Geometry(bool visible = true):visible(visible){}
	virtual ~Geometry(){

	}
	virtual void SetVisibility(bool visibility){
		visible = visibility;
	}
	virtual bool IsVisible()const{
		return visible;
	}
};

class TriangleStrip: public Geometry{
public:
	typedef std::list<Vertex> Vertices;
private:
	std::list<Vertex> vertices;
public:
	TriangleStrip(bool visible = true):Geometry(visible){
	}
	const std::list<Vertex>& GetVertices()const{
		return vertices;
	}
	void Clear(){
		vertices.empty();
	}
	void AddVertex(const Vertex& vertex){
		vertices.push_back(vertex);
	}
};

class IRenderable:public Cloneable<IRenderable>{
private:
	bool visible;
public:
	IRenderable(bool visible=true):visible(visible){}
	virtual ~IRenderable(){

	}
	virtual void SetVisibility(bool visibility){
		visible = visibility;
	}
	virtual bool IsVisible()const{
		return visible;
	}

	virtual void Render()const = 0;

};

class IRenderableFactory{
public:
	virtual boost::shared_ptr<IRenderable> CreateFromTriangleStrip(const TriangleStrip& trianglestrip) = 0;
};



class GeometryProperty: public SceneNodeProperty{
private:
	boost::shared_ptr<Geometry> geometry;
public:
	GeometryProperty(boost::shared_ptr<Geometry> geometry):geometry(geometry){}

	virtual boost::shared_ptr<SceneNodeProperty> Clone()const{
		return boost::shared_ptr<GeometryProperty>(new GeometryProperty(*this));
	}
	virtual boost::shared_ptr<GeometryProperty> ConcreteClone()const{
		return boost::shared_ptr<GeometryProperty>(new GeometryProperty(*this));
	}

	Geometry& GetGeometry(){
		return *geometry;
	}
	virtual ~GeometryProperty(){
	}
	const Geometry& GetGeometry()const{
		return *geometry;
	}
};

class TextureProperty: public SceneNodeProperty{
private:
	std::string pathtotexture;
public:
	TextureProperty(std::string pathtotexture):pathtotexture(pathtotexture){}

	virtual boost::shared_ptr<SceneNodeProperty> Clone()const{
		return boost::shared_ptr<TextureProperty>(new TextureProperty(*this));
	}
	virtual boost::shared_ptr<TextureProperty> ConcreteClone()const{
		return boost::shared_ptr<TextureProperty>(new TextureProperty(*this));
	}

	virtual ~TextureProperty(){}

	const std::string& GetPathToTexture()const{
		return pathtotexture;
	}
};

class RenderableProperty: public SceneNodeProperty{
private:
	boost::shared_ptr<IRenderable> renderable;
	boost::shared_ptr<ITexture> texture;
public:
	RenderableProperty(boost::shared_ptr<IRenderable> renderable, boost::shared_ptr<ITexture> texture=boost::shared_ptr<ITexture>((ITexture*)0)):renderable(renderable), texture(texture){

	}
	RenderableProperty(const RenderableProperty& renderableproperty):renderable(renderableproperty.renderable->Clone()), texture(renderableproperty.texture->Clone()){
		
		
	}
	virtual ~RenderableProperty(){
	}
	virtual boost::shared_ptr<SceneNodeProperty> Clone()const{
		return boost::shared_ptr<RenderableProperty>(new RenderableProperty(*this));
	}
	virtual boost::shared_ptr<RenderableProperty> ConcreteClone()const{
		return boost::shared_ptr<RenderableProperty>(new RenderableProperty(*this));
	}
	IRenderable& GetRenderable(){
		return *renderable;
	}
	const IRenderable& GetRenderable()const{
		return *renderable;
	}
	void Render()const{
		renderable->Render();
	}
	void Render(){
		renderable->Render();
	}
	const ITexture& GetTexture()const{
		return *texture;
	}
	void BindTexture()const{
		if(texture){
			texture->Bind();
		}
	}
};

class PositionProperty: public SceneNodeProperty{
private:
	Matrix4 position;
public:
	PositionProperty(){
		for(int i=0; i<4; i++){
			for(int j=0;j<4;j++){
					position(i,j) = 0.0f;
			}
		}
		for(int i=0; i<4; i++){
			position(i,i) = 1.0f;
		}
	}
	virtual boost::shared_ptr<SceneNodeProperty> Clone()const{
		return boost::shared_ptr<PositionProperty>(new PositionProperty(*this));
	}
	virtual boost::shared_ptr<PositionProperty> ConcreteClone()const{
		return boost::shared_ptr<PositionProperty>(new PositionProperty(*this));
	}
	Matrix4& GetPosition(){
		return position;
	}
	const Matrix4& GetPosition()const{
		return position;
	}
	void SetPosition(const Matrix4& position){
		this->position = position;
	}
};
class IAnimation{
public:
	enum State{Playing, Paused, Stopped};
	virtual ~IAnimation(){

	}
	virtual const std::string GetType()const = 0;
	virtual State GetState()const=0;
	virtual const std::string& GetName()const = 0;
	virtual void Animate(SceneNode& scenenode, float tf, float dtf) = 0;
	virtual void Play() = 0;
	virtual void Pause() = 0;
	virtual void Stop() = 0;
};

class AnimationsProperty: public SceneNodeProperty{
private:
	//name -> (type, IAnimation)
	std::map<std::string, boost::shared_ptr<IAnimation> > animations;
	//type -> IAnimation
	std::map<std::string, boost::shared_ptr<IAnimation> > selectedanimations;
public:
	AnimationsProperty(){}
	AnimationsProperty(const std::map<std::string, boost::shared_ptr<IAnimation> >& animations):animations(animations){}
	virtual ~AnimationsProperty(){
	}
	virtual boost::shared_ptr<SceneNodeProperty> Clone()const{
		return boost::shared_ptr<AnimationsProperty>(new AnimationsProperty(*this));
	}
	boost::shared_ptr<AnimationsProperty> ConcreteClone()const{
		return boost::shared_ptr<AnimationsProperty>(new AnimationsProperty(*this));
	}
	std::map<std::string, boost::shared_ptr<IAnimation> >& GetAnimations(){
		return animations;
	}
	const std::map<std::string, boost::shared_ptr<IAnimation> >& GetAnimations()const{
		return animations;
	}
	void AddAnimation(boost::shared_ptr<IAnimation> animation){
		///animations.insert(std::map<std::string, boost::shared_ptr<IAnimation> >::value_type(name, animation));
		animations[animation->GetName()]=animation;

		//if replacing an animation,
		if(selectedanimations.find(animation->GetType()) != selectedanimations.end() && selectedanimations.find(animation->GetType())->second->GetName() == animation->GetName()){
			selectedanimations[animation->GetType()] = animation;
		}
	}
	void SelectAnimation(const std::string& name){
		std::map<std::string, boost::shared_ptr<IAnimation> >::iterator itr = animations.find(name);
		if(itr != animations.end()){
			selectedanimations[itr->second->GetType()] = itr->second;
		}
	}
	/*std::map<std::string, boost::shared_ptr<IAnimation> >& GetSelectedAnimations(){
		return selectedanimations;
	}*/
	const std::map<std::string, boost::shared_ptr<IAnimation> >& GetSelectedAnimations()const{
		return selectedanimations;
	}
	boost::shared_ptr<IAnimation> GetAnimationByName(const std::string& name){
		return animations.find(name)->second;
	}
};

class IPhysical{
public:
	virtual ~IPhysical(){

	}
};
class PhysicsProperty: public SceneNodeProperty{
private:
	IPhysical& physical;
public:
	PhysicsProperty(IPhysical& physical):physical(physical){}
	virtual ~PhysicsProperty(){
		delete &physical;
	}
	virtual boost::shared_ptr<SceneNodeProperty> Clone()const{
		return boost::shared_ptr<PhysicsProperty>(new PhysicsProperty(*this));
	}
	virtual boost::shared_ptr<PhysicsProperty> ConcreteClone()const{
		return boost::shared_ptr<PhysicsProperty>(new PhysicsProperty(*this));
	}
	IPhysical& GetPhysical(){
		return physical;
	}
	const IPhysical& GetPhysical()const{
		return physical;
	}
	IPhysical* GetPhysicalPtr(){
		return &physical;
	}
	const IPhysical* GetPhysicalPtr()const{
		return &physical;
	}
};
