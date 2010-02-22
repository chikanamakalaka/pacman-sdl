/*
 * SceneNodeProperties.hpp
 *
 *  Created on: May 6, 2009
 *      Author: asantos
 */


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
};
class IAnimation{
public:
	virtual ~IAnimation(){

	}
	virtual void Animate(PositionProperty& positionproperty, float tf, float dtf) = 0;
};

class AnimationProperty: public SceneNodeProperty{
private:
	IAnimation& animation;
public:
	AnimationProperty(IAnimation& animation):animation(animation){}
	virtual ~AnimationProperty(){
		delete &animation;
	}
	virtual boost::shared_ptr<SceneNodeProperty> Clone()const{
		return boost::shared_ptr<AnimationProperty>(new AnimationProperty(*this));
	}
	boost::shared_ptr<AnimationProperty> ConcreteClone()const{
		return boost::shared_ptr<AnimationProperty>(new AnimationProperty(*this));
	}
	IAnimation& GetAnimation(){
		return animation;
	}
	const IAnimation& GetAnimation()const{
		return animation;
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
